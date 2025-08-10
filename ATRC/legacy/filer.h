/*+++
This project is licensed under the BSD 2-Clause License.
See the LICENSE file in the project root for license information.

Header file for the ATRC (Advanced Tagged Resource Configuration Library) library.
  This file contains the declarations for the ATRC library, which provides functionality
for reading, writing, and manipulating configuration files in a structured format.
  This library is designed to be used in both C and C++ projects, with a focus on
ease of use and flexibility.

Author(s): 
    Antonako1

Maintained at https://github.com/Antonako1/ATRC

---*/
#ifndef FILER_H
#define FILER_H
#include "./ATRC.h"

#define INVALID_INDEX ((uint64_t)-1)

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
#define ERR_INVALID_SAVE_ACTION         116
#define ERR_INVALID_PREPROCESSOR_FLAG_CONTENTS 117
#define ERR_INVALID_INCLUDE_FILE        118
#define ERR_INVALID_RAW_STRING          119

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
#define ERR_EMPTY_COMMENT               308
// File
#define ERR_CLASS_STDLIB                401

// Error types
#define ERR_STDLIB_CAST_ERROR           402
#define ERR_INVALID_POWER_VALUE         403
#define ERR_INVALID_EXPRESSION          404
#define ERR_INVALID_NUMBER              405
// File
#define ERR_CLASS_MEMORY                501

// Error types
#define ERR_MEMORY_ALLOCATION_FAILED    502
#define ERR_NULL_POINTER				503

#ifdef __cplusplus
#include <string>
#include <vector>
#include <cctype>
#include <iostream>
#include <memory>
#include <algorithm>
#include <variant>
#include <cmath>
#include <unordered_map>
#include <stdexcept>
#include <legacy.h>

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
	WRITE_COMMENT, // Deprecated, use WRITE_COMMENT_TO_BOTTOM or WRITE_COMMENT_TO_TOP
    WRITE_COMMENT_TO_BOTTOM,
    WRITE_COMMENT_TO_TOP,
};
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
	WRITE_COMMENT, // Deprecated, use WRITE_COMMENT_TO_BOTTOM or WRITE_COMMENT_TO_TOP
    WRITE_COMMENT_TO_BOTTOM,
    WRITE_COMMENT_TO_TOP,
} ATRC_SAVE;
#endif // __cplusplus


/*+++
C functions
---*/
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <string.h>

#ifdef WIN32
#define __STRDUP(src) \
    _strdup(src)
#else
#define __STRDUP(src) \
    strdup(src)
#endif

bool _ATRC_WRAP_READ(PATRC_FD a, const char* b, ReadMode mode);
void _ATRC_WRAP_ERRORMSG(int a, int b, const char *c, const char *d);
void _ATRC_WRAP__W_SAVE(
    PATRC_FD self, 
    const size_t action,
    const size_t xtra_info,
    const char *varname, 
    const char *xtra_info4,
    const char *xtra_info5
);
void _ATRC_WRAP_INSERTVAR(char* b, const char** v);
char* _ATRC_WRAP_INSERTVAR_S(const char* b, const char** c);

void Destroy_ATRC_FD_Blocks_And_Keys(PATRC_FD self);
void Destroy_ATRC_FD_Variables(PATRC_FD self);



#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#ifdef __cplusplus
bool BlockContainsKey(std::vector<CPP_Key>& keys, const CPP_Key& key);
bool BlockContainsBlock(std::vector<CPP_Block>& blocks,const CPP_Block& block);
bool VariableContainsVariable(std::vector<CPP_Variable>& variables, const CPP_Variable& variable);
std::string ParseLineSTRINGtoATRC(const std::string &line);

typedef struct _REUSABLE {
    uint64_t line_number = (uint64_t)-1;
    std::string filename;
} REUSABLE, * P_REUSABLE;


#define RAW_STR_VAR 0
#define RAW_STR_KEY 1
#define RAW_STR_NONE 2

#define RAW_STR_ACTIVE 1
#define RAW_STR_INACTIVE 0
#define CREATE_RAW_STRING 2
typedef struct RAW_STRING {
    std::string content = "";
    int is_active = RAW_STR_INACTIVE;
    int type = RAW_STR_NONE;
} RAW_STRING, * P_RAW_STRING;


void ParseLineValueATRCtoSTRING(
    std::string& line, 
    const REUSABLE &reus,
    const std::vector<CPP_Variable> &variables,
    RAW_STRING &raw_str
);

ATRC_API void _W_Save_(
CPP_ATRC_FD *filedata = nullptr,
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


enum class MATH_TOKEN_TYPE {
    NUMBER,
    OPERATOR
};

enum class MATH_NUMBER_TYPE {
    UNSOLVED,
    FLOAT,
    HEXADECIMAL,
    BINARY,
    PI,
    E,
    GOLDEN_RATIO,
    LOG2E,
    LOG10E,
    SQRT2,
};

enum class MATH_OPERATOR_TYPE {
    ADD,
    SUBTRACT,
    MULTIPLY,
    DIVIDE,
    MODULO,
    POWER,
    OPEN_PAREN,
    CLOSE_PAREN,

    SQRT,
    SQRT2,
    ABS,
    LOG,
    LOG10,

    SIN,
    COS,
    TAN,

    ASIN,
    ACOS,
    ATAN,

    LOG2E,
    LOG10E,
};

#define ATRC_PI             (double)3.14159265358979323846
#define ATRC_E              (double)2.71828182845904523536
#define ATRC_GOLDEN_RATIO   (double)1.61803398874989484820
#define ATRC_SQRT2          (double)1.41421356237309504880
#define ATRC_LOG2E          (double)1.44269504088896340736
#define ATRC_LOG10E         (double)0.43429448190325182765

struct MathToken {
    MATH_TOKEN_TYPE type;
    std::variant<double, MATH_OPERATOR_TYPE> value;
    MATH_NUMBER_TYPE numberType = MATH_NUMBER_TYPE::UNSOLVED;
};

class MathParser {
public:
    double evaluate(const std::string& expression);

private:
    std::vector<MathToken> tokenize(const std::string& input);
    std::vector<MathToken> toRPN(const std::vector<MathToken>& tokens);
    double evaluateRPN(const std::vector<MathToken>& rpn);
};


#define linecheck(line_number) (line_number == -1 ? "~unknown~" : std::to_string(line_number))

inline void errormsg(int err_num=-1, 
                    int line_number=-1, const 
                    std::string& var_name="",
                    std::string filename="no_filename"
                    ){
    std::string msg = "";
    int err_class = -1;
    switch(err_num){
        case ERR_NULL_POINTER:
            msg = "Null pointer exception at line " + linecheck(line_number);
			err_class = ERR_CLASS_MEMORY;
			break;
        case ERR_INVALID_RAW_STRING:
            msg = "Invalid raw string at or close to: '" + var_name + "' at line " + linecheck(line_number);
            err_class = ERR_CLASS_FILEHANDLER;
            break;
        case ERR_INVALID_INCLUDE_FILE:
            msg = "Invalid include file: '" + var_name + "' at line " + linecheck(line_number);
            err_class = ERR_CLASS_FILEHANDLER;
            break;
        case ERR_INVALID_NUMBER:
            msg = "Invalid number at or close to: '" + var_name + "' at line " + linecheck(line_number);
            err_class = ERR_CLASS_STDLIB;
            break;
	    case ERR_MEMORY_ALLOCATION_FAILED:
		    msg = "Memory allocation failed at line " + linecheck(line_number);
		    err_class = ERR_CLASS_MEMORY;
		    break;
        case ERR_INVALID_EXPRESSION:
            msg = "Invalid math expression at or close to: '" + var_name + "' at line " + linecheck(line_number);
            err_class = ERR_CLASS_STDLIB;
            break;
        case ERR_STDLIB_CAST_ERROR:
            msg = "Unsuccesfull cast to other type in a STDLIB function: '" + var_name + "' at line " + linecheck(line_number);
            err_class = ERR_CLASS_STDLIB;
            break;
        case ERR_INVALID_POWER_VALUE:
            msg = "Invalid power value at or close to: '" + var_name + "' at line " + linecheck(line_number);
            err_class = ERR_CLASS_STDLIB;
            break;
        case ERR_INVALID_BLOCK_DECL:
            msg = "Invalid block declaration at line " + linecheck(line_number) + ". Stopping parsing";
            err_class = ERR_CLASS_FILEHANDLER;
            break;
        case ERR_WRITECHECK:
            msg = "Writecheck creation failed: '" + var_name + "' at line " + linecheck(line_number);
            err_class = ERR_CLASS_FILEHANDLER;
            break;
        case ERR_INVALID_PREPROCESSOR_FLAG_CONTENTS:
            msg = "Invalid preprocessor flag contents: '" + var_name + "' at line " + linecheck(line_number);
            err_class = ERR_CLASS_FILEHANDLER;
            break;
        case ERR_INVALID_PREPROCESSOR_FLAG:
            msg = "Invalid preprocessor flag: '" + var_name + "' at line " + linecheck(line_number);
            err_class = ERR_CLASS_FILEHANDLER;
            break;
        case ERR_INVALID_PREPROCESSOR_TAG:
            msg = "Invalid preprocessor tag: '" + var_name + "' at line " + linecheck(line_number);
            err_class = ERR_CLASS_FILEHANDLER;
            break;
        case ERR_INVALID_PREPROCESSOR_VALUE:
            msg = "Invalid preprocessor value: '" + var_name + "' at line " + linecheck(line_number);
            err_class = ERR_CLASS_FILEHANDLER;
            break;
        case ERR_INVALID_PREPROCESSOR_SYNTAX:
            msg = "Invalid preprocessor syntax: '" + var_name + "' at line " + linecheck(line_number);
            err_class = ERR_CLASS_FILEHANDLER;
            break;
        case ERR_INVALID_KEY_DECL:
            msg = "Invalid key declaration at line " + linecheck(line_number);
            err_class = ERR_CLASS_FILEHANDLER;
            break;
        case ERR_NO_VAR_VECTOR:
            msg = "No variable vector found at line " + linecheck(line_number);
            err_class = ERR_CLASS_FILEHANDLER;
            break;
        case FILE_MODE_ERR:
            msg = "Error with file mode at line " + linecheck(line_number);
            err_class = ERR_CLASS_FILEHANDLER;
            break;
        case ERR_REREFERENCED_VAR:
            msg = "Re-Rereferenced variable: '" + var_name + "' at line " + linecheck(line_number);
            err_class = ERR_CLASS_FILEHANDLER;
            break;
        case ERR_REREFERENCED_BLOCK:
            msg = "Re-referenced block: '" + var_name + "' at line " + linecheck(line_number);
            err_class = ERR_CLASS_FILEHANDLER;
            break;
        case ERR_REREFERENCED_KEY:
            msg = "Re-Referenced key: '" + var_name + "' at line " + linecheck(line_number);
            err_class = ERR_CLASS_FILEHANDLER;
            break;
        case ERR_INSERT_WRONG:
            msg = "Invalid insert variable declaration: '" + var_name + "' at line " + linecheck(line_number);
            err_class = ERR_INSERT_VAR;
            break;
        case ERR_UNAUTHORIZED_ACCESS_TO_VAR:
            msg = "Unauthorized access to variable: '" + var_name + "' at line " + linecheck(line_number);
            err_class = ERR_CLASS_READER;
            break;
        case ERR_BLOCK_NOT_FOUND:
            msg = "Block not found: '" + var_name + "' at line " + linecheck(line_number);
            err_class = ERR_CLASS_SAVER;
            break;
        case ERR_KEY_NOT_FOUND:
            msg = "Key not found: '" + var_name + "' at line " + linecheck(line_number);
            err_class = ERR_CLASS_SAVER;
            break;
        case ERR_VAR_NOT_FOUND:
            msg = "Variable not found: '" + var_name + "' at line " + linecheck(line_number);
            err_class = ERR_CLASS_SAVER;
            break;
        case ERR_BLOCK_EXISTS:
            msg = "Block already exists: '" + var_name + "' at line " + linecheck(line_number);
            err_class = ERR_CLASS_SAVER;
            break;
        case ERR_KEY_EXISTS:
            msg = "Key already exists: '" + var_name + "' at line " + linecheck(line_number);
            err_class = ERR_CLASS_SAVER;
            break;
        case ERR_VAR_EXISTS:
            msg = "Variable already exists: '" + var_name + "' at line " + linecheck(line_number);
            err_class = ERR_CLASS_SAVER;
            break;
        case ERR_INVALID_FILE:
            msg = "Error with filename or fileheader: '" + var_name + "' at line " + linecheck(line_number);
            err_class = ERR_CLASS_FILEHANDLER;
            break;
        case ERR_INVALID_SAVE_ACTION:
            msg = "Invalid save action: '" + var_name + "' at line " + linecheck(line_number);
            err_class = ERR_CLASS_SAVER;
            break;
        default:
            msg = "Unknown error at line " + linecheck(line_number);
            break;
    }
    std::cerr << "~ATRC~Error<" << err_class << "?" << err_num << ">"<< "<" << filename << ">" <<": " << msg << std::endl;
}

ATRC_API 
bool ParseFile
(
    const std::string &_filename, 
    const std::string &encoding, 
    const std::string &extension,
    std::vector<CPP_Variable> &variables,
    std::vector<CPP_Block> &blocks
);
#endif // __cplusplus
#endif // FILER_H