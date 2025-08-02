#include "./include/ATRC/ATRC.h"
#include "./include/filer.h"
#include <iostream>
#include <fstream>
#include <utility>
#include <string>
#include <filesystem>
#include <stack>
#include <vector>
#include <unordered_set>

static void ParseLineValueATRCtoSTRING(std::string& line, const int &line_number, const std::unique_ptr<std::vector<Variable>> &variables, const std::string filename);

void check_variable_add(const std::string &trim_line, std::unique_ptr<std::vector<Variable>> &variables, const int &line_number, const std::string &filename){
    bool _is_public = false;
    if(trim_line[0] == '%') _is_public = true;
    // parse name
    size_t _equ_pos = trim_line.find('=');
    if(_equ_pos == std::string::npos){
        errormsg(ERR_INVALID_VAR_DECL, line_number, "", filename);
        return;
    }

    Variable _variable;
    // from %var% = value
    // to %var%
    // to var
    std::string _name = trim_line.substr(0, _equ_pos);
    trim(_name);
    _variable.IsPublic = _is_public;
    // true = 1, false = 0
    _variable.Name = _name.substr(2- (size_t)_is_public, _name.length() - (3 - (size_t)_is_public));

    if (_variable.Name.empty() || _variable.Name == "*") {
        errormsg(ERR_INVALID_VAR_DECL, line_number, _variable.Name, filename);
        return;
    }
    if(VariableContainsVariable(variables, _variable)) {
        errormsg(ERR_REREFERENCED_VAR, line_number, _variable.Name, filename);
        return;
    }
    std::string _value = trim_line.substr(_equ_pos + 1);
    ParseLineValueATRCtoSTRING(_value, line_number, variables, filename);
    _variable.Value = _value;
    variables->push_back(_variable);
}

void check_key_add(
const std::string &_line_trim,
const int &line_number,
std::unique_ptr<std::vector<Block>> &blocks,
std::unique_ptr<std::vector<Variable>> &variables,
const std::string &filename
)
{
    std::string _key_name = "";
    std::string _key_value = "";
    size_t _equ_pos = _line_trim.find('=');
    if(_equ_pos == std::string::npos) {
        errormsg(ERR_INVALID_KEY_DECL, line_number, "", filename);
        return;
    }
    _key_name = _line_trim.substr(0, _equ_pos);
    trim(_key_name);
    _key_value = _line_trim.substr(_equ_pos + 1);
    ParseLineValueATRCtoSTRING(_key_value, line_number, variables, filename);
    Key _key;
    _key.Name = _key_name;
    _key.Value = _key_value;
    if (blocks.get()->size() == 0) {
        errormsg(ERR_INVALID_KEY_DECL, line_number, _key.Name, filename);
        return;
    }
    if(BlockContainsKey(blocks.get()->back().Keys, _key)) {
        errormsg(ERR_REREFERENCED_KEY, line_number, _key.Name, filename);
        return;
    }
    blocks->back().Keys.push_back(_key);
}

void check_block_add(
    const std::string &_curr_block,
    std::unique_ptr<std::vector<Block>> &blocks,
    const int &line_number,
    const std::string &filename
){
    Block block;
    block.Name = _curr_block;
    if(BlockContainsBlock(blocks,block)) {
        errormsg(ERR_REREFERENCED_BLOCK, line_number, block.Name, filename);
        return;
    }
    blocks->push_back(block);
}
#define checkblock_success  1
#define checkblock_failure  0
#define checkblock_fatal    2
/// #define checkblock_success  1 
/// #define checkblock_failure  0 
/// #define checkblock_fatal    2 
// when fatal, it will show error

static int checkBlock(std::string& _curr_block, const std::string& line, int line_number = -1, std::string filename = "no_filename") {
    if(line[0] != '[') return checkblock_failure;
    size_t _end_pos = line.find(']');
    if(_end_pos == std::string::npos) {
        errormsg(ERR_INVALID_BLOCK_DECL, line_number, "", filename);
        return checkblock_fatal;
    }
    _curr_block = line.substr(1, _end_pos - 1);
    return checkblock_success;
}

std::string ParseLineSTRINGtoATRC(const std::string &line){
    /*+++
    # -> \#
    & -> \&
    % -> \%
    %var% -> %var%
    %*[int] -> %*[int]%
    ---*/

    std::string res = "";
    std::string buffer = "";
    bool in_var = false;
    for(const char &c : line){
        if(in_var){
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

std::string gen_random(const int len) {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    std::string tmp_s;
    tmp_s.reserve((size_t)len);

    for (int i = 0; i < len; ++i) {
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    
    return tmp_s;
}

/**
 * reserve characters (needs \ before them):
 * %, #, &
 * reserved sequences:
 * %*%
 * %*<int>%
 * 
 * function trims line input 
 * **line** will be replaced with the value
*/

static void _W_ParseLineValueATRCtoSTRING(
    std::string& line, const int &line_number, 
    const std::vector<Variable> *variables,
    const std::string filename
){
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

        if((!_last_is_re_dash && c == '%') || _looking_for_var) { // Variable
            if(!_looking_for_var && c == '%') {
                _looking_for_var = true;
                continue;
            }
            if(_looking_for_var && c == '%') {
                if(variables == nullptr) {
                    errormsg(ERR_NO_VAR_VECTOR, line_number, "", filename);
                    return;
                }
                if(_var_name.empty()) {
                    errormsg(ERR_INVALID_VAR_DECL, line_number, "", filename);
                    return;
                }
                if(_var_name[0] == '*') {
                    _value += "%"+_var_name+"%";
                    _var_name = "";
                    _looking_for_var = false;
                    continue;
                }
                for (Variable var : *variables) {
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

static void ParseLineValueATRCtoSTRING(std::string& line, const int &line_number, const std::unique_ptr<std::vector<Variable>> &variables, const std::string filename) {
    _W_ParseLineValueATRCtoSTRING(line, line_number, variables.get(), filename);
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

bool evaluateLogicalOperator(const std::string &op, std::list<bool> &results, size_t index) {
    if (op == "NOT") {
        if (results.empty()) return false; // Syntax error
        bool value = results.back();
        results.pop_back();
        return !value;
    }

    if (results.size() < 2) return false; // Syntax error

    bool rhs = results.back();
    results.pop_back();
    bool lhs = results.back();
    results.pop_back();

    if (op == "AND") return lhs && rhs;
    if (op == "OR") return lhs || rhs;

    return false; // Unsupported operator
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
};

/*
    ".IF",      // If               -> .IF <tag> <tag> ...
    ".ELSEIF",  // Else if          -> .ELSEIF <tag> <tag> ...
    ".ELSE",    // Else             -> .ELSE
    ".ENDIF",   // End if           -> .ENDIF
    
    ".DEFINE",  // Define           -> .DEFINE <var>=<value>
    ".UNDEF",   // Undefine         -> .UNDEF <var>

    ".LOG",     // Log to console   -> .LOG <message>           // Logs to stdout, no color
    ".WARNING", // Warning message  -> .WARNING <message>       // Logs to stdout, yellow
    ".ERROR",   // Error message    -> .ERROR <message>         // Logs to stderr, red
    ".ERRORCOUT", // Error message  -> .ERRORCOUT <message>     // Logs to stdout, red
    ".SUCCESS", // Success message  -> .SUCCESS <message>       // Logs to stdout, green
    ".DEBUG",   // Debug message    -> .DEBUG <message>         // Logs to stdout, cyan
    
    ".IGNORE",  // Ignore n lines   -> .IGNORE <n>

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
*/
const std::unordered_set<std::string> PREPROCESSOR_FLAGS = {
    ".IF", ".ELSEIF", ".ELSE", ".ENDIF", 
    ".DEFINE", ".UNDEF", 
    ".LOG", ".WARNING", ".ERROR", ".ERRORCOUT", ".SUCCESS", ".DEBUG",
    ".IGNORE"
    };
const std::unordered_set<std::string> PREPROCESSOR_TAGS = {"LINUX", "WINDOWS", "MACOS", "UNIX", "X86", "X64", "ARM", "ARM64", "AND", "OR", "NOT"};

// Struct: Preprocessor block metadata
struct PreprocessorBlock {
    std::string flag;           // Preprocessor flag
    std::string flag_contents;  // Contents following the flag
    bool isSolved = false;      // Whether the condition is solved
    bool solvedResult = false;  // Result of the solved condition
    int64_t numeral_data = 0;   // Numeric data (e.g., for .IGNORE)
    std::string string_data;    // String data (e.g., for .LOG)
    std::string var_name;       // Variable name (e.g., for .DEFINE)
    std::string var_value;      // Variable value (e.g., for .DEFINE)
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
PPRes parsePreprocessorFlag(const std::string &line, PreprocessorBlock &block, const std::string &filename) {
    std::string lineCopy = line + " ";
    size_t spacePos = lineCopy.find_first_of(' ');

    if (spacePos == std::string::npos) {
        errormsg(ERR_INVALID_PREPROCESSOR_FLAG, -1, line, filename);
        return PPRes::InvalidFlag;
    }

    block.flag = lineCopy.substr(1, spacePos - 1);
    str_to_upper_s(block.flag);
    if (PREPROCESSOR_FLAGS.find(block.flag) == PREPROCESSOR_FLAGS.end()) {
        errormsg(ERR_INVALID_PREPROCESSOR_FLAG, -1, block.flag, filename);
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
    str_to_upper_s(block.flag_contents);
    if (block.flag == ".IF" || block.flag == ".ELSEIF") {
        std::vector<std::string> tags = split(block.flag_contents, ' ');
        if (tags.empty()) {
            errormsg(ERR_INVALID_PREPROCESSOR_SYNTAX, -1, line, filename);
            return PPRes::InvalidFlagContents;
        }

        // Evaluate tags and logical expressions
        std::list<bool> results;

        std::string logical_operator = "";
        uint64_t wait_for_next_value = 0;
        for(size_t i = 0; i < tags.size(); i++){
            std::string &tag = tags[i];
            if (PREPROCESSOR_TAGS.find(tag) != PREPROCESSOR_TAGS.end()) {
                if (tag == "LINUX" || tag == "WINDOWS" || tag == "MACOS" || tag == "UNIX")
                    results.push_back(evaluatePlatformTag(tag));
                else if (tag == "X86" || tag == "X64" || tag == "ARM" || tag == "ARM64")
                    results.push_back(evaluateArchitectureTag(tag));
                else {
                    if(tag == "NOT"){
                        wait_for_next_value++;
                        logical_operator = tag;
                        continue;
                    } else if(tag == "AND" || tag == "OR"){
                        if(results.empty()){
                            errormsg(ERR_INVALID_PREPROCESSOR_SYNTAX, -1, line, filename);
                            return PPRes::InvalidFlagContents;
                        }
                        wait_for_next_value++;
                        logical_operator = tag;
                        continue;
                    }
                }
                if(wait_for_next_value-- == 1){
                    if(logical_operator.empty()){
                        errormsg(ERR_INVALID_PREPROCESSOR_TAG, -1, tag, filename);
                        return PPRes::InvalidFlagContents;
                    }
                    results.push_back(evaluateLogicalOperator(logical_operator, results, i));
                    wait_for_next_value = 0;
                    logical_operator = "";
                }
            } else {
                errormsg(ERR_INVALID_PREPROCESSOR_TAG, -1, tag, filename);
                return PPRes::InvalidFlagContents;
            }
        }


        block.isSolved = true;
        block.solvedResult = results.back();
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
                errormsg(ERR_INVALID_PREPROCESSOR_VALUE, -1, line, filename);
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
            errormsg(ERR_INVALID_PREPROCESSOR_VALUE, -1, line, filename);
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

    return PPRes::Normal;
}







std::pair<
    std::unique_ptr<std::vector<Variable>>, 
    std::unique_ptr<std::vector<Block>>
> 
ParseFile
(
    const std::string &_filename, 
    const std::string &encoding, 
    const std::string &extension
) {
    std::unique_ptr<std::vector<Variable>> variables = std::make_unique<std::vector<Variable>>();
    std::unique_ptr<std::vector<Block>>  blocks = std::make_unique<std::vector<Block>>();
    std::vector<Key> keys;
    std::vector<std::pair<std::string, std::string>> definitions;
    if(encoding == "" || extension == ""){} // Ignore for now
    std::string filename = _filename;
    if(!std::filesystem::exists(filename)){
        errormsg(ERR_INVALID_FILE, -1, "File does not exist: " + filename, filename);
        return std::make_pair(std::move(variables), std::move(blocks));
    }
    std::ifstream file(filename);
    if (!file.is_open()) {
        errormsg(ERR_INVALID_FILE, -1, "Failed opening: " + filename, filename);
        file.close();
        return std::make_pair(std::move(variables), std::move(blocks));
    }
    // Read through the file line by line
    std::string line;
    int line_number = 0;
    std::string _curr_block = "";
    

    std::stack<PreprocessorBlock> _preprocessor_blocks;
    bool inside_preprocessor_block = false;
    bool solved_preprocessor_block = false;
    bool wait_for_new_block = false;
    int64_t _ignore_lines = 0;

    while (std::getline(file, line)) {
        if(line_number++ == 0){
            std::string CUR_HEADER = line;
            trim(CUR_HEADER);
            std::string FL_HEADER = "#!ATRC";
            if(CUR_HEADER != FL_HEADER){
                errormsg(ERR_INVALID_FILE, -1, "Invalid file header: " + filename, filename);
                file.close();
                return std::make_pair(std::move(variables), std::move(blocks));
            }
            continue;
        }
        std::string _line_trim = line;
        trim(_line_trim);

        // Skip empty lines and comments
        if (_line_trim.empty()) continue;
        if (_line_trim[0] == '#' 
        || inside_preprocessor_block
        || solved_preprocessor_block
        || wait_for_new_block
        ) 
        {   
            // Line is a preprocessor block
            if(_line_trim[0] == '#' && _line_trim[1] == '.'){
                PreprocessorBlock _block;
                PPRes _res = parsePreprocessorFlag(_line_trim.substr(0), _block, filename);
                switch(_res) {
                    case PPRes::SeeContents: 
                        if(solved_preprocessor_block) break;
                        if(_block.isSolved && _block.solvedResult) {
                            inside_preprocessor_block = true;   // Start reading lines
                            solved_preprocessor_block = true;   // Block is solved
                            wait_for_new_block = false;
                            _preprocessor_blocks.push(_block);
                        }
                        break;
                    case PPRes::ThreeParts:
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
                        break;
                    case PPRes::Undefine:
                        for(auto &def : definitions) {
                            if(def.first == _block.var_name) {
                                def.second = "";
                                continue;
                            }
                        }
                        break;
                    case PPRes::DontSeeContents:
                        wait_for_new_block = true;
                        continue;
                    case PPRes::EndIF: 
                        wait_for_new_block = false;
                        inside_preprocessor_block = false;
                        solved_preprocessor_block = false;
                        if(_preprocessor_blocks.empty()) {
                            errormsg(ERR_INVALID_PREPROCESSOR_SYNTAX, -1, line, filename);
                            file.close();
                            return std::make_pair(std::move(variables), std::move(blocks));
                        }
                        break;
                    case PPRes::Message: 
                        if(_block.flag == ".LOG")       std::cout << _block.string_data << std::endl;
                        else if(_block.flag == ".SUCCESS")   std::cout << ANSI_COLOR_GREEN << _block.string_data << ANSI_COLOR_RESET << std::endl;
                        else if(_block.flag == ".DEBUG")     std::cout << ANSI_COLOR_CYAN << _block.string_data << ANSI_COLOR_RESET << std::endl;
                        else if(_block.flag == ".WARNING")   std::cout << ANSI_COLOR_YELLOW<< _block.string_data << ANSI_COLOR_RESET << std::endl;
                        else if(_block.flag == ".ERROR")     std::cerr << ANSI_COLOR_RED << _block.string_data << ANSI_COLOR_RESET << std::endl;
                        else if(_block.flag == ".ERRORCOUT") std::cout << ANSI_COLOR_RED << _block.string_data << ANSI_COLOR_RESET << std::endl;
                        else break;
                    case PPRes::Ignore: 
                        _ignore_lines = _block.numeral_data;
                        break;
                    // Error handling, logging made in parse_preprocessor_flag()
                    case PPRes::InvalidFlag: 
                    case PPRes::InvalidFlagContents:
                    default: 
                        file.close();
                        return std::make_pair(std::make_unique<std::vector<Variable>>(), std::make_unique<std::vector<Block>>());
                        break;  
                }
            } 
            
            else if (!wait_for_new_block){ // Inside preprocessor block
                PreprocessorBlock *_block = &_preprocessor_blocks.top();
                _block->lines.push_back(_line_trim);
            }
            // If the line is a comment, skip it
            continue;
        }
        
        if(_ignore_lines > 0) {
            _ignore_lines--;
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
                        if(line.empty()) continue;
                        // Check if line is a variable
                        if(line[0] == '%' || line.substr(0, 2) == "<%"){
                            check_variable_add(line, variables, line_number, filename);
                            continue;
                        }
                        // Check if line is a block
                        int check = checkBlock(_curr_block, line, line_number, filename);
                        if(check == checkblock_fatal){
                            file.close();
                            return std::make_pair(std::move(variables), std::move(blocks));
                        }
                        if (check == checkblock_success) {
                            check_block_add(_curr_block, blocks, line_number, filename);
                            continue;
                        }
                        check_key_add(line, line_number, blocks, variables, filename);
                    }
                }
                _preprocessor_blocks.pop(); // pop after processing
            }
        }

        // Check if line is a variable
        if(_line_trim[0] == '%' || _line_trim.substr(0, 2) == "<%"){
            check_variable_add(_line_trim, variables, line_number, filename);
            continue;
        }
        // Check if line is a block
        int check = checkBlock(_curr_block, _line_trim, line_number, filename);
        if(check == checkblock_fatal){
            file.close();
            return std::make_pair(std::move(variables), std::move(blocks));
        }
        if (check == checkblock_success) {
            check_block_add(_curr_block, blocks, line_number, filename);
            continue;
        }
        check_key_add(_line_trim, line_number, blocks, variables, filename);
    }

    file.close();

    // Will throw error if no blocks or variables are found
    if(variables->empty()){
        Variable _var;
        _var.Name = "var-" + gen_random(10);
        _var.Value = "val-" + gen_random(10);
        variables->push_back(_var);
    } 
    if(blocks->empty()) {
        // append a random block with randomized name
        Block _block;
        _block.Name = "block-" + gen_random(10);
        blocks->push_back(_block);
    }
    return std::make_pair(std::move(variables), std::move(blocks));
}













void save_final_data(const std::string &filename, const std::string &final_data){
    std::ofstream ofs(filename, std::ofstream::trunc);
    ofs << final_data;
    ofs.close();
}


void _W_Save_(
    ATRC_FD *filedata, 
    const ATRC_SAVE &action, 
    const int &xtra_info, 
    const std::string &xtra_info2,
    const std::string &xtra_info3,
    const std::string &xtra_info4
) {
    std::string line = "";
    std::string final_data = "";
    switch (action)
    {
    case ATRC_SAVE::FULL_SAVE:
        
        break;
    case ATRC_SAVE::ADD_BLOCK: {
            std::ofstream file(filedata->GetFilename(), std::ios::app);
            if (!file.is_open()) {
                errormsg(ERR_INVALID_FILE, 
                    -1, 
                    "Failed opening for saving: " + filedata->GetFilename(), 
                    filedata->GetFilename()
                    );
                file.close();
                return;
            }
        file << "\n" << "[" << xtra_info2 + "]";
        file.close();
    }
        break;
    case ATRC_SAVE::REMOVE_BLOCK: {
        std::ifstream file(filedata->GetFilename(), std::ios::out);
        if (!file.is_open()) {
            errormsg(ERR_INVALID_FILE, 
                -1, 
                "Failed opening for saving: " + filedata->GetFilename(), 
                filedata->GetFilename()
                );
            
            file.close();
            return;
        }

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
                int check = checkBlock(curr_block, line, -1, filedata->GetFilename());
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
        break;
    }
    case ATRC_SAVE::ADD_KEY:
        {
            std::ifstream file(filedata->GetFilename());
            if (!file.is_open()) {
                errormsg(ERR_INVALID_FILE, 
                    -1, 
                    "Failed opening for saving: " + filedata->GetFilename(), 
                    filedata->GetFilename()
                );
                return;
            }
            bool key_added = false;
            std::string curr_block = "";
            while (std::getline(file, line)) {
                std::string line_trim = line;
                trim(line_trim);
                if(key_added) {
                    final_data += line + "\n";
                } else {
                    if(line_trim[0] != '['){
                        final_data += line + "\n";
                        continue;
                    }
                    int check = checkBlock(curr_block, line, -1, filedata->GetFilename());
                    if(check == checkblock_failure){
                        file.close();
                        break;
                    }
                    if(check ==checkblock_success){
                        if(curr_block == xtra_info4){
                            final_data += line + "\n";
                            final_data += xtra_info2 + "=" + xtra_info3 + "\n";
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
            break;
        }
        break;
    case ATRC_SAVE::REMOVE_KEY: {
            std::ifstream file(filedata->GetFilename());
            if (!file.is_open()) {
                errormsg(ERR_INVALID_FILE, 
                    -1, 
                    "Failed opening for saving: " + filedata->GetFilename(), 
                    filedata->GetFilename()
                );
                return;
            }
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
                    int check = checkBlock(curr_block, line, -1, filedata->GetFilename());
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
        }
        break;
    case ATRC_SAVE::MODIFY_KEY: {
            std::ifstream file(filedata->GetFilename());
            if (!file.is_open()) {
                errormsg(ERR_INVALID_FILE, 
                    -1, 
                    "Failed opening for saving: " + filedata->GetFilename(), 
                    filedata->GetFilename()
                );
                return;
            }
            bool block_found = false;
            bool key_modified = false;
            std::string curr_block = "";
            while (std::getline(file, line)) {
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
                    int check = checkBlock(curr_block, line, -1, filedata->GetFilename());
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
        }
        break;
    case ATRC_SAVE::ADD_VAR:{
        std::ifstream file(filedata->GetFilename(), std::ios::app);
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
        while(std::getline(file, line)){
            if(ln_num++ == 1) final_data += xtra_info2 + "\n";
            final_data += line + "\n";
        }
        file.close();
        save_final_data(filedata->GetFilename(), final_data);
        break;
    }
    case ATRC_SAVE::REMOVE_VAR: {
        int var_line_num = 0;
        std::ifstream file(filedata->GetFilename(), std::ios::app);
        if (!file.is_open()) {
            errormsg(ERR_INVALID_FILE, 
                -1, 
                "Failed opening for saving: " + filedata->GetFilename(), 
                filedata->GetFilename()
                );
            file.close();
            return;
        }
        while(std::getline(file, line)){
            var_line_num++;
            std::string trim_line = line;
            trim(trim_line);
            if(trim_line[0] == '%'){
                size_t equ_pos = trim_line.find_first_of('=');
                std::string variable_name = trim_line.substr(0, equ_pos-1);
                trim(variable_name);
                _W_ParseLineValueATRCtoSTRING(line, var_line_num, (filedata->GetVariables()).get(), filedata->GetFilename());
                if(variable_name == "%" + xtra_info2 + "%"){
                    continue;
                }
            }
            final_data += line + "\n";
        }
        file.close();
        save_final_data(filedata->GetFilename(), final_data);
        break;
    }
    case ATRC_SAVE::MODIFY_VAR: {
        std::ifstream file(filedata->GetFilename(), std::ios::app);
        if (!file.is_open()) {
            errormsg(ERR_INVALID_FILE, 
                -1, 
                "Failed opening for saving: " + filedata->GetFilename(), 
                filedata->GetFilename()
                );
            file.close();
            return;
        }
        int var_line_num = 0;
        while(std::getline(file, line)){
            var_line_num++;
            std::string trim_line = line;
            trim(trim_line);
            if(trim_line[0] == '%'){
                size_t equ_pos = trim_line.find_first_of('=');
                std::string variable_name = trim_line.substr(0, equ_pos-1);
                trim(variable_name);
                _W_ParseLineValueATRCtoSTRING(line, var_line_num, (filedata->GetVariables()).get(), filedata->GetFilename());
                if(variable_name == "%" + xtra_info2 + "%"){
                    final_data += filedata->GetVariables()->at((size_t)xtra_info).Name +"="+ filedata->GetVariables()->at((size_t)xtra_info).Value + "\n";
                    continue;
                }
            }
            final_data += line + "\n";
        }
        file.close();
        save_final_data(filedata->GetFilename(), final_data);
        break;
    }
    }
}
