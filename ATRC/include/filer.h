#ifndef FILER_H
#define FILER_H
#include "./ATRC.h"

/*
Name            FG  BG
Black           30  40
Red             31  41
Green           32  42
Yellow          33  43
Blue            34  44
Magenta         35  45
Cyan            36  46
White           37  47
Bright Black    90  100
Bright Red      91  101
Bright Green    92  102
Bright Yellow   93  103
Bright Blue     94  104
Bright Magenta  95  105
Bright Cyan     96  106
Bright White    97  107
*/

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"


// File
#define ERR_CLASS_FILEHANDLER           100
// Error types
#define ERR_INVALID_VAR_DECL            101
#define ERR_INVALID_BLOCK_DECL          102
#define ERR_INVALID_KEY_DECL            103
#define ERR_NO_VAR_VECTOR               104
#define ERR_REREFERENCED_VAR            105
#define ERR_REREFERENCED_BLOCK          106
#define ERR_REREFERENCED_KEY            107
#define ERR_INSERT_VAR                  108  
#define ERR_INVALID_FILE                109
#define FILE_MODE_ERR                   110
#define ERR_WRITECHECK                  111
#define ERR_INVALID_PREPROCESSOR_FLAG   112
#define ERR_INVALID_PREPROCESSOR_TAG    113
#define ERR_INVALID_PREPROCESSOR_VALUE  114
#define ERR_INVALID_PREPROCESSOR_SYNTAX 115


// File
#define ERR_CLASS_READER                200

// Error types
#define ERR_UNAUTHORIZED_ACCESS_TO_VAR  201

// File
#define ERR_CLASS_SAVER                 300

// Error types
#define ERR_BLOCK_NOT_FOUND             301
#define ERR_KEY_NOT_FOUND               302
#define ERR_VAR_NOT_FOUND               303
#define ERR_BLOCK_EXISTS                304
#define ERR_KEY_EXISTS                  305
#define ERR_VAR_EXISTS                  306
#define ERR_INSERT_WRONG                307

// File
#define ERR_CLASS_STDLIB                401

// Error types
#define ERR_STDLIB_CAST_ERROR           402

// File
#define ERR_CLASS_MEMORY                501

// Error types
#define ERR_MEMORY_ALLOCATION_FAILED    502
#ifdef __cplusplus
namespace atrc {
std::string str_to_lower(const char *str);
void str_to_lower_s(std::string &str);
std::string str_to_upper(const char *str);
void str_to_upper_s(std::string &str);
std::vector<std::string> split(const std::string &str, char separator);

enum class
ATRC_SAVE{
    FULL_SAVE = -1, 
    ADD_BLOCK = 0, 
    REMOVE_BLOCK,
    ADD_KEY,
    REMOVE_KEY,
    MODIFY_KEY,
    ADD_VAR,
    REMOVE_VAR,
    MODIFY_VAR,
};
}
#else 
typedef enum {
    FULL_SAVE = -1, 
    ADD_BLOCK = 0, 
    REMOVE_BLOCK,
    ADD_KEY,
    REMOVE_KEY,
    MODIFY_KEY,
    ADD_VAR,
    REMOVE_VAR,
    MODIFY_VAR,
} ATRC_SAVE;
#endif // __cplusplus

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
bool _ATRC_WRAP_FUNC_1(C_PATRC_FD a, const char* b, ReadMode mode);
void _ATRC_WRAP_FUNC_2(int a, int b, const char *c, const char *d);
void _ATRC_WRAP_FUNC_3(
    C_PATRC_FD self, 
    const int action, 
    const int xtra_info, 
    const char *varname, 
    const char *xtra_info4,
    const char *xtra_info5
);
void _ATRC_WRAP_FUNC_4(char* b, const char** v);
char* _ATRC_WRAP_FUNC_5(const char* b, const char** c);
#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#ifdef __cplusplus
#include <string>
#include <iostream>
#include <memory>
#include <vector>
#include <algorithm>
#include <ATRC.h>
namespace atrc {
bool BlockContainsKey(std::vector<Key>& keys, const Key& key);
bool BlockContainsBlock(std::unique_ptr<std::vector<Block>>& blocks,const Block& block);
bool VariableContainsVariable(std::unique_ptr<std::vector<Variable>>& variables, const Variable& variable);
std::string ParseLineSTRINGtoATRC(const std::string &line);



ATRC_API void _W_Save_(
ATRC_FD *filedata = nullptr,
const ATRC_SAVE &action = ATRC_SAVE::FULL_SAVE, 
const int &xtra_info = -2, 
const std::string &xtra_info2 = "",
const std::string &xtra_info3 = "",
const std::string &xtra_info4 = ""
);

// trim from start (in place)
inline void ltrim(std::string &s) {
s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
    return !std::isspace(ch);
}));
}

// trim from end (in place)
inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// trim from both ends (in place)
inline void trim(std::string &s) {
    rtrim(s);
    ltrim(s);
}

inline std::string ltrim_copy(std::string s) {
    ltrim(s);
    return s;
}

inline std::string rtrim_copy(std::string s) {
    rtrim(s);
    return s;
}

inline std::string trim_copy(std::string s) {
    trim(s);
    return s;
}
// Remove newlines from string
inline void remove_newlines(std::string &s) {
    s.erase(std::remove(s.begin(), s.end(), '\n'), s.end());
}

inline void errormsg(int err_num=-1, 
                    int line_number=-1, const 
                    std::string& var_name="",
                    std::string filename="no_filename"
                    ){
    std::string msg = "";
    int err_class = -1;
    switch(err_num){
	    case ERR_MEMORY_ALLOCATION_FAILED:
		    msg = "Memory allocation failed";
		    err_class = ERR_CLASS_MEMORY;
		    break;
        case ERR_STDLIB_CAST_ERROR:
            msg = "Unsuccesfull cast to other type in a STDLIB function: '" + var_name + "' at line " + std::to_string(line_number);
            err_class = ERR_CLASS_STDLIB;
            break;
        case ERR_INVALID_VAR_DECL:
            msg = "Invalid variable declaration: '" + var_name +"' at line " + std::to_string(line_number);
            err_class = ERR_CLASS_FILEHANDLER;
            break;
        case ERR_INVALID_BLOCK_DECL:
            msg = "Invalid block declaration at line " + std::to_string(line_number) + ". Stopping parsing";
            err_class = ERR_CLASS_FILEHANDLER;
            break;
        case ERR_WRITECHECK:
            msg = "Writecheck creation failed: '" + var_name + "'";
            err_class = ERR_CLASS_FILEHANDLER;
            break;
        case ERR_INVALID_PREPROCESSOR_FLAG:
            msg = "Invalid preprocessor flag: '" + var_name + "'";
            err_class = ERR_CLASS_FILEHANDLER;
            break;
        case ERR_INVALID_PREPROCESSOR_TAG:
            msg = "Invalid preprocessor tag: '" + var_name + "'";
            err_class = ERR_CLASS_FILEHANDLER;
            break;
        case ERR_INVALID_PREPROCESSOR_VALUE:
            msg = "Invalid preprocessor value: '" + var_name + "'";
            err_class = ERR_CLASS_FILEHANDLER;
            break;
        case ERR_INVALID_PREPROCESSOR_SYNTAX:
            msg = "Invalid preprocessor syntax: '" + var_name + "'";
            err_class = ERR_CLASS_FILEHANDLER;
            break;
        case ERR_INVALID_KEY_DECL:
            msg = "Invalid key declaration at line " + std::to_string(line_number);
            err_class = ERR_CLASS_FILEHANDLER;
            break;
        case ERR_NO_VAR_VECTOR:
            msg = "No variable vector found";
            err_class = ERR_CLASS_FILEHANDLER;
            break;
        case FILE_MODE_ERR:
            msg = "Error with file mode";
            err_class = ERR_CLASS_FILEHANDLER;
            break;
        case ERR_REREFERENCED_VAR:
            msg = "Re-Rereferenced variable: '" + var_name + "' at line " + std::to_string(line_number);
            err_class = ERR_CLASS_FILEHANDLER;
            break;
        case ERR_REREFERENCED_BLOCK:
            msg = "Re-referenced block: '" + var_name + "' at line " + std::to_string(line_number);
            err_class = ERR_CLASS_FILEHANDLER;
            break;
        case ERR_REREFERENCED_KEY:
            msg = "Re-Referenced key: '" + var_name + "' at line " + std::to_string(line_number);
            err_class = ERR_CLASS_FILEHANDLER;
            break;
        case ERR_INSERT_WRONG:
            msg = "Invalid insert variable declaration: '" + var_name + "'";
            err_class = ERR_INSERT_VAR;
            break;
        case ERR_UNAUTHORIZED_ACCESS_TO_VAR:
            msg = "Unauthorized access to variable: '" + var_name + "'";
            err_class = ERR_CLASS_READER;
            break;
        case ERR_BLOCK_NOT_FOUND:
            msg = "Block not found: '" + var_name + "'";
            err_class = ERR_CLASS_SAVER;
            break;
        case ERR_KEY_NOT_FOUND:
            msg = "Key not found: '" + var_name + "'";
            err_class = ERR_CLASS_SAVER;
            break;
        case ERR_VAR_NOT_FOUND:
            msg = "Variable not found: '" + var_name + "'";
            err_class = ERR_CLASS_SAVER;
            break;
        case ERR_BLOCK_EXISTS:
            msg = "Block already exists: '" + var_name + "'";
            err_class = ERR_CLASS_SAVER;
            break;
        case ERR_KEY_EXISTS:
            msg = "Key already exists: '" + var_name + "'";
            err_class = ERR_CLASS_SAVER;
            break;
        case ERR_VAR_EXISTS:
            msg = "Variable already exists: '" + var_name + "'";
            err_class = ERR_CLASS_SAVER;
            break;
        case ERR_INVALID_FILE:
            msg = "Error with filename or fileheader: '" + var_name + "'";
            err_class = ERR_CLASS_FILEHANDLER;
            break;
        default:
            msg = "Unknown error at line " + std::to_string(line_number);
            break;
    }
    std::cerr << "~ATRC~Error<" << err_class << "?" << err_num << ">"<< "<" << filename << ">" <<": " << msg << std::endl;
}

ATRC_API 
std::pair<
    std::unique_ptr<std::vector<Variable>>, 
    std::unique_ptr<std::vector<Block>>
> 
ParseFile
(
    const std::string &filename, 
    const std::string &encoding, 
    const std::string &extension
);
} // namespace atrc
#endif // __cplusplus
#endif // FILER_H