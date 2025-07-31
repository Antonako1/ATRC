#include "ATRC.h"
#include "filer.h"
#include "legacy.h"
#include <iostream>
#include <fstream>
#include <utility>
#include <string>
#include <filesystem>
#include <stack>
#include <vector>
#include <unordered_set>
#include <list>
#include <cstdlib>
#include <stdexcept>
#define checkblock_success  1
#define checkblock_failure  0
#define checkblock_fatal    2


/*+++
Preprocessor handling functions, parsing, constants, etc.
---*/

std::string resolveValue(const std::string &value, const std::vector<CPP_Variable> &variables) {
    std::string val = value;
    if(val.empty()) return ""; // Empty value

    // Handle quoted strings
    if ((val.front() == '"' && val.back() == '"') || (val.front() == '\'' && val.back() == '\'')) {
        return val.substr(1, val.size() - 2);
    }

    // Handle booleans
    if (val == "TRUE") return "1";
    if (val == "FALSE") return "0";

    // Handle variable substitution: %VAR%
    if (val.front() == '%' && val.back() == '%') {
        std::string varName = val.substr(1, val.length() - 2);
        for (const auto &var : variables) {
            std::string temp = var.Name;
            str_to_upper_s(temp);
            str_to_upper_s(varName);
            if (temp == varName) return var.Value;
        }
        return ""; // Undefined variable = empty string
    }

    return val; // Literal fallback
}



bool evaluatePlatformTag(const std::string &tag) {
#ifdef __linux__
    return tag == "LINUX";
#elif _WIN32 || _WIN64
    return tag == "WINDOWS";
#elif __APPLE__ || __MACH__
    return tag == "MACOS";
#elif __unix__
    return tag == "UNIX";
#else
    return false;
#endif
}

bool evaluateArchitectureTag(const std::string &tag) {
#if defined(__x86_64__) || defined(_M_X64)
    return tag == "X64";
#elif defined(i386) || defined(__i386__) || defined(__i386) || defined(_M_IX86)
    return tag == "X86";
#elif defined(__aarch64__) || defined(_M_ARM64)
    return tag == "ARM64";
#elif defined(__arm__) || defined(_M_ARM)
    return tag == "ARM";
#else
    return false;
#endif
}

bool evaluateLogicalOperator(const std::string &op, std::list<std::string> &results, const std::vector<CPP_Variable> &variables) {
    if (op == "NOT") {
        if (results.empty()) return false;
        std::string value = resolveValue(results.back(), variables);
        results.pop_back();
        return value == "0";
    }
    
    if (results.size() < 2) return false;
    std::string rhs = resolveValue(results.back(), variables); results.pop_back();
    std::string lhs = resolveValue(results.back(), variables); results.pop_back();

    if (op == "AND") return lhs == "1" && rhs == "1";
    if (op == "OR") return lhs == "1" || rhs == "1";
    if (op == "EQU") return lhs == rhs;
    if (op == "NEQ") return lhs != rhs;

    // Only allow numeric comparison if both sides look like numbers
    try {
        double lnum = std::stod(lhs);
        double rnum = std::stod(rhs);
        if (op == "GT") return lnum > rnum;
        if (op == "LT") return lnum < rnum;
        if (op == "GTE") return lnum >= rnum;
        if (op == "LTE") return lnum <= rnum;
    } catch (...) {
        return false; // Type mismatch (non-numeric used in GT/LT etc.)
    }

    return false;
}



enum class PPRes {
    SeeContents,        // Flag is solved in the preprocessor block
    DontSeeContents,    // Flag is not solved in the preprocessor block
    EndIF,              // End of the preprocessor block
    Normal,             // Normal return
    Ignore,             // Ignore n lines
    InvalidFlag,        // Invalid flag
    InvalidFlagContents,// Invalid flag contents

    ThreeParts,         // .FLAG=<var>=<value>
    Undefine,           // .UNDEF <var>
    Message,            // .LOG, .WARNING, .ERROR...

    SR,              // Start raw string
    ER,              // End raw string
};

/*
    ".IF",      // If               -> .IF <tag> <tag> ...
    ".ELSEIF",  // Else if          -> .ELSEIF <tag> <tag> ...
    ".ELSE",    // Else             -> .ELSE
    ".ENDIF",   // End if           -> .ENDIF
    
    ".DEFINE",  // Define           -> .DEFINE <var>=<value>
    ".UNDEF",   // Undefine         -> .UNDEF <var>
    ".INCLUDE", // Include file     -> .INCLUDE <file>

    ".LOG",     // Log to console   -> .LOG <message>           // Logs to stdout, no color
    ".WARNING", // Warning message  -> .WARNING <message>       // Logs to stdout, yellow
    ".ERROR",   // Error message    -> .ERROR <message>         // Logs to stderr, red
    ".ERRORCOUT", // Error message  -> .ERRORCOUT <message>     // Logs to stdout, red
    ".SUCCESS", // Success message  -> .SUCCESS <message>       // Logs to stdout, green
    ".DEBUG",   // Debug message    -> .DEBUG <message>         // Logs to stdout, cyan
    
    ".IGNORE",  // Ignore n lines   -> .IGNORE <n>

    ".SR",      // Raw string       -> .R
    ".ER"

    Operating systems
    "LINUX",
    "WINDOWS",
    "MACOS",
    "UNIX",

    Architectures
    "X86",
    "X64",
    "ARM",
    "ARM64",

    Logical operators
    "OR",
    "AND",
    "NOT",
    "EQU",
    "NEQ",
    "GT",
    "LT",
    "GTE",
    "LTE",

    Boolean values
    "FALSE",
    "TRUE",

*/
const std::unordered_set<std::string> PREPROCESSOR_FLAGS = {
    ".IF", ".ELSEIF", ".ELSE", ".ENDIF", 
    ".DEFINE", ".UNDEF", ".INCLUDE",
    ".LOG", ".WARNING", ".ERROR", ".ERRORCOUT", ".SUCCESS", ".DEBUG",
    ".IGNORE", ".SR", ".ER"
};
const std::unordered_set<std::string> PREPROCESSOR_TAGS = {
    "LINUX", "WINDOWS", "MACOS", "UNIX", 
    "X86", "X64", "ARM", "ARM64", 
    "AND", "OR", "NOT", "EQU", "NEQ", "GT", "LT", "GTE", "LTE",
};

// Struct: Preprocessor block metadata
struct PreprocessorBlock {
    std::string flag;           // Preprocessor flag
    std::string flag_contents;  // Contents following the flag
    bool isSolved = false;      // Whether the condition is solved
    bool solvedResult = false;  // Result of the solved condition
    int64_t numeral_data = 0;   // Numeric data (e.g., for .IGNORE)
    std::string string_data;    // String data (e.g., for .LOG)
    std::string var_name;       // CPP_Variable name (e.g., for .DEFINE)
    std::string var_value;      // CPP_Variable value (e.g., for .DEFINE)
    std::vector<std::string> lines; // Lines within the preprocessor block
};


/*+++
Parses preprocessor line:

#<flag> <contents...>
-> 
{
    flag: <flag>
    flag_contents: <contents>
    isSolved: false|true        // Flag is solved in the preprocessor block
    solvedResult: false|true    // Result of the flag
    lines: [] // lines that are inside the preprocessor block. Will be parsed outside this function
}
---*/
PPRes parsePreprocessorFlag(const std::string &line, PreprocessorBlock &block, const REUSABLE &reus, std::vector<CPP_Variable> &variables) {
    PPRes res = PPRes::Normal;
    std::string lineCopy = line + " ";
    size_t spacePos = lineCopy.find_first_of(' ');

    if (spacePos == std::string::npos) {
        errormsg(ERR_INVALID_PREPROCESSOR_FLAG, (int)reus.line_number, line, reus.filename);
        return PPRes::InvalidFlag;
    }
    block.flag = lineCopy.substr(1, spacePos - 1);
    str_to_upper_s(block.flag);
    if (PREPROCESSOR_FLAGS.find(block.flag) == PREPROCESSOR_FLAGS.end()) {
        errormsg(ERR_INVALID_PREPROCESSOR_FLAG, (int)reus.line_number, block.flag, reus.filename);
        return PPRes::InvalidFlag;
    }


    // Handle .ENDIF
    if (block.flag == ".ENDIF") return PPRes::EndIF;
    if(block.flag == ".ELSE") {
        block.isSolved = true;
        block.solvedResult = true;
        return PPRes::SeeContents;
    };
    
    // Extract and process flag contents
    block.flag_contents = trim_copy(lineCopy.substr(spacePos + 1));
    if(block.flag == ".SR") {
        block.flag_contents = trim_copy(block.flag_contents);
        if(block.flag_contents.empty()) {
            errormsg(ERR_INVALID_PREPROCESSOR_FLAG_CONTENTS, (int)reus.line_number, line, reus.filename);
            return PPRes::InvalidFlagContents;
        }
        if(block.flag_contents == "KEY") {
            block.var_name = "KEY";
            block.numeral_data = RAW_STR_KEY;
        } else if(block.flag_contents == "VAR") {
            block.var_name = "VAR";
            block.numeral_data = RAW_STR_VAR;
        } else {
            errormsg(ERR_INVALID_PREPROCESSOR_FLAG_CONTENTS, (int)reus.line_number, line, reus.filename);
            return PPRes::InvalidFlagContents;
        }
        block.isSolved = true;
        block.solvedResult = true;
        return PPRes::SR; // Raw string start
    } else if(block.flag == ".ER") {
        block.isSolved = true;
        block.solvedResult = true;
        return PPRes::ER; // Raw string end
    }
    
    if(block.flag == ".INCLUDE") {
        block.flag_contents = trim_copy(block.flag_contents);
        block.string_data = block.flag_contents;
        return PPRes::SeeContents; // Include file contents will be processed later
    }

    // For if statements
    if (block.flag == ".IF" || block.flag == ".ELSEIF") {
        std::vector<std::string> tags = split(block.flag_contents, ' ');
        if (tags.empty()) {
            errormsg(ERR_INVALID_PREPROCESSOR_SYNTAX, (int)reus.line_number, line, reus.filename);
            return PPRes::InvalidFlagContents;
        }
        
        // Evaluate tags and logical expressions
        std::list<std::string> results;

        std::string logical_operator = "";
        uint64_t wait_for_next_value = 0;
        bool jump_to_tags = false;
        for(size_t i = 0; i < tags.size(); i++){
            std::string &tag = tags[i];
            if(tag.empty()) continue;
            if ((tag.front() == '"' && tag.back() == '"') || (tag.front() == '\'' && tag.back() == '\'')) {
                results.push_back(tag);
                if(wait_for_next_value > 0) {jump_to_tags = !jump_to_tags; continue;}
            }
            else if (PREPROCESSOR_TAGS.find(tag) != PREPROCESSOR_TAGS.end()) {
                str_to_upper_s(tag);
                if (tag == "LINUX" || tag == "WINDOWS" || tag == "MACOS" || tag == "UNIX") {
                    results.push_back(std::to_string(evaluatePlatformTag(tag)));
                    continue;
                }
                else if (tag == "X86" || tag == "X64" || tag == "ARM" || tag == "ARM64") {
                    results.push_back(std::to_string(evaluateArchitectureTag(tag)));
                    continue;
                }
                else {
                    if(tag == "NOT"){
                        wait_for_next_value++;
                        logical_operator = tag;
                        continue;
                    } else if(tag == "AND" || tag == "OR"){
                        if(results.empty()){
                            errormsg(ERR_INVALID_PREPROCESSOR_SYNTAX, (int)reus.line_number, line, reus.filename);
                            return PPRes::InvalidFlagContents;
                        }
                        wait_for_next_value += 1;
                        logical_operator = tag;
                        continue;
                    } else if(tag == "EQU" || tag == "NEQ" || tag == "GT" || tag == "LT" || tag == "GTE" || tag == "LTE") {
                        if(results.empty()){
                            errormsg(ERR_INVALID_PREPROCESSOR_SYNTAX, (int)reus.line_number, line, reus.filename);
                            return PPRes::InvalidFlagContents;
                        }
                        wait_for_next_value += 1;
                        logical_operator = tag;
                        continue;
                    }
                }
                if(wait_for_next_value-- == 1){
                    if(logical_operator.empty()){
                        errormsg(ERR_INVALID_PREPROCESSOR_TAG, (int)reus.line_number, tag, reus.filename);
                        return PPRes::InvalidFlagContents;
                    }
                    results.push_back(std::to_string(evaluateLogicalOperator(logical_operator, results, variables)));
                    wait_for_next_value = 0;
                    logical_operator = "";
                }
            } else {
                results.push_back(resolveValue(tag, variables));
                if(wait_for_next_value > 0) {jump_to_tags = !jump_to_tags; continue;}
            }
        }

        if(wait_for_next_value-- == 1){
            if(logical_operator.empty()){
                errormsg(ERR_INVALID_PREPROCESSOR_TAG, (int)reus.line_number, "~UNKNOWN~", reus.filename);
                return PPRes::InvalidFlagContents;
            }
            results.push_back(std::to_string(evaluateLogicalOperator(logical_operator, results, variables)));
            wait_for_next_value = 0;
            logical_operator = "";
        }

        block.isSolved = true;
        block.solvedResult = atrc_to_bool(results.back().c_str());
        return block.solvedResult == true ? PPRes::SeeContents : PPRes::DontSeeContents;
    }
    // Additional flag types with specific syntax
    if (block.flag == ".LOG" 
    || block.flag == ".WARNING" 
    || block.flag == ".ERROR" 
    || block.flag == ".ERRORCOUT" 
    || block.flag == ".IGNORE"
    || block.flag == ".SUCCESS"
    || block.flag == ".DEBUG"
    ) {
        if(block.flag == ".IGNORE") {
            block.numeral_data = std::stoll(block.flag_contents);
            if(block.numeral_data < 0) {
                errormsg(ERR_INVALID_PREPROCESSOR_VALUE, (int)reus.line_number, line, reus.filename);
                return PPRes::InvalidFlagContents;
            }
            return PPRes::Ignore;
        } else {
            block.string_data = block.flag_contents;
            return PPRes::Message;
        }
    }

    else if (block.flag == ".DEFINE") {
        
        size_t eqPos = block.flag_contents.find('=');
        if (eqPos == std::string::npos) {
            errormsg(ERR_INVALID_PREPROCESSOR_VALUE, (int)reus.line_number, line, reus.filename);
            return PPRes::InvalidFlagContents;
        }
        block.var_name = trim_copy(block.flag_contents.substr(0, eqPos));
        block.var_value = trim_copy(block.flag_contents.substr(eqPos + 1));
        return PPRes::ThreeParts;
    }
    else if(block.flag == ".UNDEF") {
        block.var_name = block.flag_contents;
        return PPRes::Undefine;
    }

    return res;
}

/*+++
Parsing functions for ATRC
---*/


void _W_ParseLineValueATRCtoSTRING(std::string& line, const REUSABLE &reus, const std::vector<CPP_Variable> &variables) {
    trim(line);
    bool _last_is_re_dash = false;
    bool _looking_for_var = false;
    std::string _value = "";
    std::string _var_name = "";
    for (const char &c : line) {
        if(c == '\\' && _last_is_re_dash) {
            _value += '\\';
            _last_is_re_dash = false;
            continue;
        }
        if (c == '\\') {
            _last_is_re_dash = true;
            continue;
        }   
        
        if(!_last_is_re_dash && c == '#') { // Comment
            _value = _value.substr(0, _value.length() - 1);
            trim(_value);
            break; 
        }
        if(!_last_is_re_dash && c == '&') { // Whitespace
            _value += ' ';
            continue;
        }

        if((!_last_is_re_dash && c == '%') || _looking_for_var) { // CPP_Variable
            if(!_looking_for_var && c == '%') {
                _looking_for_var = true;
                continue;
            }
            if(_looking_for_var && c == '%') {
                if(variables.empty()) {
                    errormsg(ERR_NO_VAR_VECTOR, (int)reus.line_number, "", reus.filename);
                    return;
                }
                if(_var_name.empty()) {
                    errormsg(ERR_INVALID_VAR_DECL, (int)reus.line_number, "", reus.filename);
                    return;
                }
                if(_var_name[0] == '*') {
                    _value += "%"+_var_name+"%";
                    _var_name = "";
                    _looking_for_var = false;
                    continue;
                }
                for (const CPP_Variable &var : variables) {
                    if(var.Name == _var_name) {
                        _value += var.Value;
                        break;
                    }
                }
                _var_name = "";
                _looking_for_var = false;
                continue;
            }
            _var_name += c;
            continue;
        }
        _value += c;
        _last_is_re_dash = false;
    }

    line = _value;
}
void ParseLineValueATRCtoSTRING(std::string& line, const REUSABLE &reus, const std::vector<CPP_Variable> &variables, RAW_STRING &raw_str) {
    if(raw_str.is_active == RAW_STR_INACTIVE || raw_str.is_active == RAW_STR_ACTIVE) {
        _W_ParseLineValueATRCtoSTRING(line, reus, variables);
    }
}

std::string ParseLineSTRINGtoATRC(const std::string &line) {
    /*+++
    # -> \#
    & -> \&
    % -> \%
    %var% -> %var%
    %*[int] -> %*[int]%
    */
    std::string res = "";
    std::string buffer = "";
    bool in_var = false;
    for (const char &c : line) {
        if(in_var) {
            buffer += c;
            if(c == '%'){
                in_var = false;
                res += buffer;
                buffer = "";
            }
            continue;
        } else {
            if(c == '%'){
                buffer += '%';
                in_var = true;
                continue;
            }
            else if(c == '#'){
                res += "\\#";
                continue;
            }
            else if(c == '&'){
                res += "\\&";
                continue;
            }
        }
        res += c;
    }
    if(buffer[0] == '%'){
        res += '\\' + buffer;
    }
    return res;
}

bool check_and_add_block(std::string &curr_block, std::vector<CPP_Block> &blocks, REUSABLE &reus){
    for(auto &block : blocks) {
        if(block.Name == curr_block) {
            errormsg(ERR_REREFERENCED_BLOCK, (int)reus.line_number, block.Name, reus.filename);
            return false;
        }
    }
    CPP_Block _block;
    _block.Name = curr_block;
    _block.line_number = reus.line_number;
    blocks.push_back(_block);
    if(blocks.size() == (size_t)-1) {
        errormsg(ERR_INVALID_BLOCK_DECL, (int)reus.line_number, curr_block, reus.filename);
        return false;
    }
    return true;
}
bool check_and_add_key(std::string &line_trim, std::vector<CPP_Block> &blocks, REUSABLE &reus, std::vector<CPP_Variable> &vars, RAW_STRING &raw_str){
    if(blocks.empty()) {
        errormsg(ERR_INVALID_KEY_DECL, (int)reus.line_number, "", reus.filename);
        return false;
    }
    
    for(auto &block : blocks) {
        if(block.Name == line_trim) {
            errormsg(ERR_REREFERENCED_KEY, (int)reus.line_number, block.Name, reus.filename);
            return false;
        }
    }
    std::string _key_name = "";
    std::string _key_value = "";
    size_t _equ_pos = line_trim.find('=');
    if(_equ_pos == std::string::npos) {
        errormsg(ERR_INVALID_KEY_DECL, (int)reus.line_number, "", reus.filename);
        return false;
    }
    _key_name = line_trim.substr(0, _equ_pos);
    trim(_key_name);
    _key_value = line_trim.substr(_equ_pos + 1);
    if(raw_str.is_active == CREATE_RAW_STRING || raw_str.is_active == RAW_STR_INACTIVE) {
        ParseLineValueATRCtoSTRING(_key_value, reus, vars, raw_str);
    }
    CPP_Key _key;
    _key.Name = _key_name;
    _key.Value = _key_value;
    _key.line_number = reus.line_number;
    size_t last_block_index = blocks.size() - 1;
    try {
        _key.enum_value = blocks[last_block_index].Keys.size();
    } catch (const std::exception &e) {
        e; // Suppress unused variable warning
        errormsg(ERR_INVALID_BLOCK_DECL, (int)reus.line_number, _key_name, reus.filename);
        return false;
    }
    try {
        blocks[last_block_index].Keys.push_back(_key);
    } catch (const std::exception &e) {
        e; // Suppress unused variable warning
        errormsg(ERR_INVALID_KEY_DECL, (int)reus.line_number, _key_name, reus.filename);
        return false;
    }
    return true;
}
bool check_and_add_variable(std::string &line, std::vector<CPP_Variable> &variables, REUSABLE &reus, RAW_STRING &raw_str){
    CPP_Variable var;
    var.IsPublic = false;
    if(line[0] == '%') var.IsPublic = true;
    size_t equ = line.find('=');
    if(equ == std::string::npos) {
        errormsg(ERR_INVALID_VAR_DECL, (int)reus.line_number, "", reus.filename);
        return false;
    }

    var.Name = line.substr(2 - (size_t)var.IsPublic, equ - (3 - (size_t)var.IsPublic));
    if(var.Name.empty() || var.Name == "*") {
        errormsg(ERR_INVALID_VAR_DECL, (int)reus.line_number, var.Name, reus.filename);
        return false;
    }
    if(VariableContainsVariable(variables, var)) {
        errormsg(ERR_REREFERENCED_VAR, (int)reus.line_number, var.Name, reus.filename);
        return false;
    }
    var.Value = line.substr(equ + 1);
    var.line_number = reus.line_number;
    if(raw_str.is_active != CREATE_RAW_STRING) {
        ParseLineValueATRCtoSTRING(var.Value, reus, variables, raw_str);
    }
    variables.push_back(var);
    return true;
}
uint32_t check_for_block_declaration(std::string &_curr_block, const std::string &line, REUSABLE &reus) {
    if(line[0] != '[') return checkblock_failure;
    size_t end_pos = line.find(']');
    if(end_pos == std::string::npos) {
        errormsg(ERR_INVALID_BLOCK_DECL, (int)reus.line_number, "", reus.filename);
        return checkblock_fatal;
    }
    _curr_block = line.substr(1, end_pos - 1);
    return checkblock_success;
}

#define ATRC_CONTINUE 1
#define ATRC_RETURN_FALSE 2
#define ATRC_RETURN_TRUE 3
int end_of_life_add(std::vector<CPP_Block> &blocks, std::vector<CPP_Variable> &variables, REUSABLE &reus, std::string &_line_trim, std::string &_curr_block, RAW_STRING &raw_str) {
        // check for variable declaration
        if(_line_trim[0] == '%' || _line_trim.substr(0, 2) == "<%") {
            check_and_add_variable(_line_trim, variables, reus, raw_str);
            return ATRC_CONTINUE;
        }
        // check for block declaration
        uint32_t check = check_for_block_declaration(_curr_block, _line_trim, reus);
        if(check == checkblock_fatal){
            return ATRC_RETURN_FALSE;
        }
        if (check == checkblock_success) {
            check_and_add_block(_curr_block, blocks, reus);
            return ATRC_CONTINUE;
        }
        // check and add key to block
        check_and_add_key(_line_trim, blocks, reus, variables, raw_str);
        return ATRC_RETURN_TRUE;
}

bool ParseFile(const std::string &_filename, const std::string &encoding, const std::string &extension, std::vector<CPP_Variable> &variables,std::vector<CPP_Block> &blocks) {

    // This is used to hold #.INCLUDE files.
    // Once #.INCLUDE is detected, the file will be read and the variables and blocks will be added to this vector.
    // And can be used to access the variables and blocks from the included files.
    std::vector<
        std::pair<
            std::vector<CPP_Variable>, 
            std::vector<CPP_Block>
        > 
    > filedatas;

    std::vector< std::pair<std::string, std::string> > definitions;
    if(encoding == "" || extension == ""){} // Ignore for now
    std::string filename = _filename;
    if(!std::filesystem::exists(filename)){
        errormsg(ERR_INVALID_FILE, -1, "File does not exist: " + filename, filename);
        return false;
    }
    std::ifstream file(filename);
    if (!file.is_open()) {
        errormsg(ERR_INVALID_FILE, -1, "Failed opening: " + filename, filename);
        file.close();
        return false;
    }
    RAW_STRING raw_str;
    REUSABLE REUSABLE;
    REUSABLE.line_number = 0;
    REUSABLE.filename = filename;

    // Read through the file line by line
    std::string line;
    std::string _curr_block = "";

    std::stack<PreprocessorBlock> _preprocessor_blocks; // Holds preprocessor blocks (for if statements, etc.)
    bool inside_preprocessor_block = false;
    bool solved_preprocessor_block = false;
    bool wait_for_new_block = false;
    uint64_t _ignore_lines = 0;
    
    while (std::getline(file, line)) {
        std::string _line_trim = line;
        trim(_line_trim);
        if(REUSABLE.line_number++ == 0){
            std::string FL_HEADER = "#!ATRC";
            if(_line_trim != FL_HEADER){
                errormsg(ERR_INVALID_FILE, -1, "Invalid file header: " + filename + " Is: '" + _line_trim+"'", filename);
                file.close();
                return false;
            }
            continue;
        }
        if(_ignore_lines > 0) { //preprocessor ignoring
            _ignore_lines--;
            continue;
        }
        if (_line_trim.empty()) continue;
        if (_line_trim[0] == '#' 
        || inside_preprocessor_block
        || solved_preprocessor_block
        || wait_for_new_block
        ) {
            // check if line is a preprocessor directive
            if(_line_trim.length() > 1 && (_line_trim[0] == '#' && _line_trim[1] == '.')) {
                // Handle preprocessor directive
                PreprocessorBlock _block;
                PPRes _res = parsePreprocessorFlag(_line_trim.substr(0), _block, REUSABLE, variables);
                switch(_res){
                    case PPRes::SR:
                        raw_str.is_active = CREATE_RAW_STRING; // Start raw string
                        raw_str.type = (int)_block.numeral_data; // Set type to RAW_STR_KEY or RAW_STR_VAR
                        raw_str.content = ""; // Reset content
                        break;
                    case PPRes::ER: 
                        if(raw_str.type == RAW_STR_KEY) {
                            blocks.back().Keys.back().Value += "\n" + raw_str.content;
                            // Remove the last newline character if it exists
                            if(!blocks.back().Keys.back().Value.empty() && blocks.back().Keys.back().Value.back() == '\n')
                                blocks.back().Keys.back().Value.pop_back();
                            
                            ParseLineValueATRCtoSTRING(blocks.back().Keys.back().Value, REUSABLE, variables, raw_str);
                        } else if(raw_str.type == RAW_STR_VAR) {
                            variables.back().Value += "\n" + raw_str.content;

                            // Remove the last newline character if it exists
                            if(!variables.back().Value.empty() && variables.back().Value.back() == '\n')
                                variables.back().Value.pop_back();

                            ParseLineValueATRCtoSTRING(variables.back().Value, REUSABLE, variables, raw_str);
                        } else {
                            errormsg(ERR_INVALID_RAW_STRING, (int)REUSABLE.line_number, _line_trim, REUSABLE.filename);
                            file.close();
                            return false;
                        }
                    raw_str.is_active = RAW_STR_INACTIVE; // End raw string
                        raw_str.type = RAW_STR_NONE;
                        raw_str.content = "";
                        break;
                    case PPRes::SeeContents: {
                        if(_block.flag == ".INCLUDE") {
                            std::vector<CPP_Variable> _vars = std::vector<CPP_Variable>();
                            std::vector<CPP_Block> _blocks = std::vector<CPP_Block>();
                            std::string include_filename = _block.string_data;
                            if(include_filename.empty()) {
                                errormsg(ERR_INVALID_INCLUDE_FILE, (int)REUSABLE.line_number, _line_trim, REUSABLE.filename);
                                file.close();
                                return false;
                            }
                            if(!std::filesystem::exists(include_filename)) {
                                errormsg(ERR_INVALID_INCLUDE_FILE, (int)REUSABLE.line_number, include_filename, REUSABLE.filename);
                                file.close();
                                return false;
                            }
                            bool include_result = ParseFile(include_filename, encoding, extension, _vars, _blocks);
                            filedatas.push_back(std::make_pair(_vars, _blocks));
                            if(!include_result) {
                                errormsg(ERR_INVALID_INCLUDE_FILE, (int)REUSABLE.line_number, include_filename, REUSABLE.filename);
                                file.close();
                                return false;
                            }
                            // Add variables and blocks from the included file to the main variables and blocks
                            for (auto var : _vars) {
                                if(!VariableContainsVariable(variables, var)) {
                                    variables.push_back(var);
                                }
                            }
                            for (auto block : _blocks) {
                                if(!BlockContainsBlock(blocks, block)) {
                                    blocks.push_back(block);
                                }
                            }
                        } else {
                            if(solved_preprocessor_block) break;
                            if(_block.isSolved && _block.solvedResult) {
                                inside_preprocessor_block = true;   // Start reading lines
                                solved_preprocessor_block = true;   // CPP_Block is solved
                                wait_for_new_block = false;
                                _preprocessor_blocks.push(_block);
                            }
                        }
                    } break;
                    case PPRes::DontSeeContents: {
                        wait_for_new_block = true;
                    } continue;
                    case PPRes::EndIF: {
                        wait_for_new_block = false;
                        inside_preprocessor_block = false;
                        solved_preprocessor_block = false;
                        if(_preprocessor_blocks.empty()) {
                            if(_block.flag != ".ENDIF") {
                                errormsg(ERR_INVALID_PREPROCESSOR_FLAG, (int)REUSABLE.line_number, _line_trim, REUSABLE.filename);
                                file.close();
                                return false;
                            }
                        }
                    } break;
                    case PPRes::Normal: break;
                    case PPRes::Ignore: {
                        _ignore_lines = _block.numeral_data;
                    } break;
                    case PPRes::ThreeParts: {
                        if(_block.flag == ".DEFINE") {
                            for(auto &def : definitions) {
                                if(def.first == _block.var_name) {
                                    def.second = _block.var_value;
                                    continue;
                                }
                            }
                            std::pair<std::string, std::string> _def;
                            _def.first = _block.var_name;
                            _def.second = _block.var_value;
                            definitions.push_back(_def);
                        }
                    } break;
                    case PPRes::Undefine: {
                        for(auto &def : definitions) {
                            if(def.first == _block.var_name) {
                                def.second = "";
                                continue;
                            }
                        }
                    } break;
                    case PPRes::Message: {
                        if(wait_for_new_block) break;
                        if(_block.flag == ".LOG")       std::cout << _block.string_data << std::endl;
                        else if(_block.flag == ".SUCCESS")   std::cout << ANSI_COLOR_GREEN << _block.string_data << ANSI_COLOR_RESET << std::endl;
                        else if(_block.flag == ".DEBUG")     std::cout << ANSI_COLOR_CYAN << _block.string_data << ANSI_COLOR_RESET << std::endl;
                        else if(_block.flag == ".WARNING")   std::cout << ANSI_COLOR_YELLOW<< _block.string_data << ANSI_COLOR_RESET << std::endl;
                        else if(_block.flag == ".ERROR")     std::cerr << ANSI_COLOR_RED << _block.string_data << ANSI_COLOR_RESET << std::endl;
                        else if(_block.flag == ".ERRORCOUT") std::cout << ANSI_COLOR_RED << _block.string_data << ANSI_COLOR_RESET << std::endl;
                    } break;
                    case PPRes::InvalidFlag: {
                        errormsg(ERR_INVALID_PREPROCESSOR_FLAG, (int)REUSABLE.line_number, line, REUSABLE.filename);
                        file.close();
                    } return false;
                    case PPRes::InvalidFlagContents: {
                        errormsg(ERR_INVALID_PREPROCESSOR_FLAG_CONTENTS, (int)REUSABLE.line_number, line, REUSABLE.filename);
                        file.close();
                    } return false;
                    default : {
                        errormsg(ERR_INVALID_PREPROCESSOR_FLAG, (int)REUSABLE.line_number, line, REUSABLE.filename);
                        file.close();
                    } return false;
                }
            } else if(_line_trim[0] == '#' && _line_trim[1] != '.'){
                // Just a plain comment... skip it.
            }
            else if (!wait_for_new_block){ // Inside preprocessor block
                PreprocessorBlock _block = _preprocessor_blocks.top();
                _block.lines.push_back(_line_trim);
            }
            continue;
        }
        
        if(_preprocessor_blocks.size() > 0){
            for(size_t i = 0; i < _preprocessor_blocks.size(); i++){
                PreprocessorBlock *_block = &_preprocessor_blocks.top();
                if(
                    (_block->isSolved && _block->solvedResult) &&
                    (_block->flag == ".IF" || _block->flag == ".ELSEIF" || _block->flag == ".ELSE")
                ) {
                    for (std::string &line : _block->lines) {
                        trim(line);
                        int results = end_of_life_add(blocks, variables, REUSABLE, line, _curr_block, raw_str);
                        if(results == ATRC_RETURN_FALSE) {
                            file.close();
                            return false;
                        } else if(results == ATRC_CONTINUE) {
                            continue;
                        } else {}
                    }
                }
                _preprocessor_blocks.pop(); // pop after processing
            }
        }

        if(raw_str.is_active == CREATE_RAW_STRING || raw_str.is_active == RAW_STR_ACTIVE) {
            if(raw_str.is_active != CREATE_RAW_STRING) {
                if(_line_trim.empty()) {
                    continue; // skip empty lines
                }
                raw_str.content += line + "\n";
                continue;
            }
        }

        int results = end_of_life_add(blocks, variables, REUSABLE, line, _curr_block, raw_str);
        if(raw_str.is_active == CREATE_RAW_STRING) raw_str.is_active = RAW_STR_ACTIVE;
        if(results == ATRC_RETURN_FALSE) {
            file.close();
            return false;
        } else if(results == ATRC_CONTINUE) {
            continue;
        } else {}
    }
    file.close();
    return true;
}

/*+++
File saving functions
---*/
void save_final_data(const std::string &filename, const std::string &final_data){
    std::ofstream ofs(filename, std::ofstream::trunc);
    ofs << final_data;
    ofs.close();
}


void add_to_top_of_file(std::string line_to_add, std::string &final_data, const std::vector<char> &add_after_chars, std::ifstream &file, uint64_t &ln_num) {
    bool added = false;
    std::string line;
    while(std::getline(file, line)){
        std::string line_trim = line;
        trim(line_trim);
        if(ln_num++ == 0) {
            final_data += line + "\n";
            continue;
        }
        bool skip_line = false;
        for(char c : add_after_chars) {
            if(line_trim[0] == c || line_trim.empty()) {
                final_data += line + "\n";
                skip_line = true;
                break;
            }
        }
        if(skip_line) continue;
        if(!added) {
            final_data += line_to_add + "\n";
            added = true;
        }
        final_data += line + "\n";
    }
    if(!added) {
        final_data += line_to_add + "\n";
        added = true;
    }
}

void _W_Save_(CPP_ATRC_FD *filedata, const ATRC_SAVE &action, const int &xtra_info, const std::string &xtra_info2,const std::string &xtra_info3,const std::string &xtra_info4){
    std::string line = "";
    std::string final_data = "";
    std::ifstream file(filedata->GetFilename(), std::ios::app);
    REUSABLE reus;
    reus.line_number = -1;
    reus.filename = filedata->GetFilename();
    if (!file.is_open()) {
        errormsg(ERR_INVALID_FILE, 
            -1, 
            "Failed opening for saving: " + filedata->GetFilename(), 
            filedata->GetFilename()
            );
        file.close();
        return;
    }
    uint64_t ln_num = 0;
    switch (action) {
    case ATRC_SAVE::FULL_SAVE: {} break;
    case ATRC_SAVE::ADD_BLOCK: {
        std::string block = "[" + xtra_info2 + "]\n";
        add_to_top_of_file(block, final_data, {'#', '%', '<', ' '}, file, ln_num);
        file.close(); 
        save_final_data(filedata->GetFilename(), final_data);
    } break;
    case ATRC_SAVE::REMOVE_BLOCK: {
        bool block_found_ = false;
        bool block_removed = false;
        std::string curr_block = "";
        while(std::getline(file, line)){
            std::string line_trim = line;
            trim(line_trim);
            if(block_removed){
                final_data += line + "\n";
            } else {
                if(line_trim[0] != '['){
                    if(!block_removed && block_found_){
                        continue;
                    }
                    final_data += line + "\n";
                    continue;
                }
                int check = check_for_block_declaration(curr_block, line, reus);
                if(check == checkblock_failure){
                    file.close();
                    break;
                }
                if(check ==checkblock_success){
                    if(block_found_){
                        block_removed = true;
                    }
                    if(curr_block == xtra_info2){
                        block_found_ = true;
                    } else {
                        final_data += line + "\n";
                    }
                }
            }   
        }
        file.close();
        save_final_data(filedata->GetFilename(), final_data);
    } break;
    case ATRC_SAVE::ADD_KEY: {
        bool key_added = false;
        std::string curr_block = "";
        std::string key_name = xtra_info2;
        std::string key_value = xtra_info3;
        std::string block_name = xtra_info4;
        while(std::getline(file, line)){
            std::string line_trim = line;
            trim(line_trim);
            if(key_added) {
                final_data += line + "\n";
            } else {
                if(line_trim[0] != '['){
                    final_data += line + "\n";
                    continue;
                }
                int check = check_for_block_declaration(curr_block, line, reus);
                if(check == checkblock_failure){
                    file.close();
                    break;
                }
                if(check ==checkblock_success){
                    if(curr_block == block_name){
                        final_data += line + "\n";
                        final_data += key_name + "=" + key_value + "\n";
                        key_added = true;
                        continue;
                    } else {
                        final_data += line + "\n";
                        continue;
                    }
                }
            }
        }
        file.close();
        save_final_data(filedata->GetFilename(), final_data);
    } break;
    case ATRC_SAVE::REMOVE_KEY: {
        bool block_found = false;
        bool key_removed = false;
        std::string curr_block = "";
        while (std::getline(file, line)) {
            std::string line_trim = line;
            trim(line_trim);
            if(key_removed) {
                final_data += line + "\n";
            } else {
                if(block_found){
                    size_t equ_pos = line_trim.find_first_of('=');
                    std::string key_name = line_trim.substr(0, equ_pos);
                    trim(key_name);
                    if(key_name == xtra_info2){
                        key_removed = true;
                        continue;
                    } else{
                        final_data += line + "\n";
                    }
                    continue;
                }
                if(line_trim[0] != '['){
                    final_data += line + "\n";
                    continue;
                }
                int check = check_for_block_declaration(curr_block, line, reus);
                if(check == checkblock_failure){
                    file.close();
                    break;
                }
                if(check == checkblock_success){
                    if(curr_block == xtra_info4){
                        block_found = true;
                        final_data += line + "\n";
                        continue;
                    } else {
                        final_data += line + "\n";
                        continue;
                    }
                }
            }
        }
        file.close();
        save_final_data(filedata->GetFilename(), final_data);
    } break;
    case ATRC_SAVE::MODIFY_KEY: {
        bool block_found = false;
        bool key_modified = false;
        std::string curr_block = "";

        while(std::getline(file, line)){
            std::string line_trim = line;
            trim(line_trim);
            if(key_modified) {
                final_data += line + "\n";
            } else {
                if(block_found){
                    size_t equ_pos = line_trim.find_first_of('=');
                    std::string key_name = line_trim.substr(0, equ_pos);
                    trim(key_name);
                    if(key_name == xtra_info2){
                        final_data += xtra_info2+"="+xtra_info3+"\n";
                        continue;
                    } else{
                        final_data += line + "\n";
                    }
                    continue;
                }
                if(line_trim[0] != '['){
                    final_data += line + "\n";
                    continue;
                }
                int check = check_for_block_declaration(curr_block, line, reus);
                if(check == checkblock_failure){
                    file.close();
                    break;
                }
                if(check ==checkblock_success){
                    if(curr_block == xtra_info4){
                        block_found = true;
                        final_data += line + "\n";
                        continue;
                    } else {
                        final_data += line + "\n";
                        continue;
                    }
                }
            }
        }

        file.close();
        save_final_data(filedata->GetFilename(), final_data);
    } break;
    case ATRC_SAVE::ADD_VAR: {
        std::string var_line = xtra_info2+"\n";
        add_to_top_of_file(var_line, final_data, {'#', ' '}, file, ln_num);
        file.close(); 
        save_final_data(filedata->GetFilename(), final_data);
    } break;
    case ATRC_SAVE::REMOVE_VAR: {
        int var_line_num = 0;
        bool var_found = false;
        while(std::getline(file, line)){
            reus.line_number = var_line_num++;
            if(var_found){
                final_data += line + "\n";
                continue;
            }
            std::string trim_line = line;
            trim(trim_line);
            if(trim_line[0] == '%'){
                size_t equ_pos = trim_line.find_first_of('=');
                std::string variable_name = trim_line.substr(0, equ_pos);
                // Remove the '%' at the start and end of the variable name
                trim(variable_name);
                // Remove the '%' at the end of the variable name
                if(variable_name.back() == '=') {
                    variable_name.pop_back();
                }
                if(variable_name.back() == '%') {
                    variable_name.pop_back();
                }
                if(variable_name.front() == '%') {
                    variable_name.erase(0, 1); // Remove the '%' at the start
                }
                if(variable_name == xtra_info2){
                    var_found = true;
                    continue;
                }
            }
            final_data += line + "\n";
        }
        file.close();
        save_final_data(filedata->GetFilename(), final_data);
    } break;
    case ATRC_SAVE::MODIFY_VAR: {
        int var_line_num = 0;
        bool var_found = false;
        while(std::getline(file, line)){
            reus.line_number = var_line_num++;
            std::string trim_line = line;
            trim(trim_line);
            if(trim_line[0] == '%'){
                size_t equ_pos = trim_line.find_first_of('=');
                std::string variable_name = trim_line.substr(0, equ_pos);
                trim(variable_name);
                // Remove the '%' at the start and end of the variable name
                if(variable_name.back() == '=') {
                    variable_name.pop_back();
                }
                if(variable_name.back() == '%') {
                    variable_name.pop_back();
                }
                if(variable_name.front() == '%') {
                    variable_name.erase(0, 1); // Remove the '%' at the start
                }
                std::vector<CPP_Variable> args = filedata->Variables;
                if(variable_name == xtra_info2){
                    if(args.at((size_t)xtra_info).IsPublic == false) {
                        var_found = true;
                        final_data += line + "\n";
                        continue;
                    }
                    final_data += '%' + args.at((size_t)xtra_info).Name +"%="+ args.at((size_t)xtra_info).Value + "\n";
                    var_found = true;
                    continue;
                }
            }
            final_data += line + "\n";
        }
        file.close();
        save_final_data(filedata->GetFilename(), final_data);
    } break;
    case ATRC_SAVE::WRITE_COMMENT_TO_BOTTOM: {
        // Added to the bottom of the file
        std::string comment = "#" + xtra_info2;
        while(std::getline(file, line)){
            final_data += line + "\n";
        }
        final_data += comment + "\n";
        file.close();
        save_final_data(filedata->GetFilename(), final_data);
    } break;
    case ATRC_SAVE::WRITE_COMMENT_TO_TOP: {
        // Added to the top of the file
        std::string comment = "#" + xtra_info2;
        add_to_top_of_file(comment, final_data, {}, file, ln_num);
        file.close();
        save_final_data(filedata->GetFilename(), final_data);
    } break;
    default:
        errormsg(ERR_INVALID_SAVE_ACTION, -1, "", filedata->GetFilename());
        break;
    }
    if(file.is_open()) file.close();
}