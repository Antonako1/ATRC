/**
 * @file ATRC.h
 * @brief Header file for the ATRC (Advanced Tagged Resource Configuration Library) library.
 * This library provides a simple and efficient way to manage configuration files
 * in a structured format.
 * @version 3.0_67
 * 
 * @details
 * This project is licensed under the BSD 2-Clause License.  
 * See the LICENSE file in the project root for license information.
 * 
 * Author(s): Antonako1  
 * Maintained at: https://github.com/Antonako1/ATRC
 *
 * ## Usage
 * To use the ATRC library, include this header file in your source code:
 * ```c
 * #define ATRC_IMPLEMENTATION // Define this in one source file only
 * #include <ATRC/ATRC.h>
 * ```
 * 
 * ## Preprocessor Definitions
 *
 * Define these macros along with `ATRC_IMPLEMENTATION` in one source file to include the implementation and/or modify the behavior of the ATRC library.
 * You can define the following macros before including this header.
 * These definitions control the behavior of the ATRC library.
 * 
 * ### Implementation Definitions
 * - `ATRC_IMPLEMENTATION`  
 *   Define this in **one source file only** to include the implementation of the ATRC library.
 * 
 * #### Memory Management
 * - `ATRC_USE_STACK`  
 *   Define this to use stack allocation for memory management. This is the default behavior.
 *   If you define this, the library will use stack-allocated buffers for strings, blocks, and variables.
 *   This is useful for performance and memory efficiency, but it limits the size of the data.
 * 
 * - `ATRC_USE_HEAP`
 *   Define this to use heap allocation for memory management.
 * 
 * - `ATRC_STACK_MAX_STRING_LENGTH`
 *   Define this to set the maximum string length for stack-allocated strings. 
 *   This include block, key and variable names, values and list values and injection results.
 *   128 characters is the default.
 * 
 * - `ATRC_STACK_MAX_STRING_ARRAY_LENGTH`
 *   Define this to set the maximum number of strings in a stack-allocated string array.
 *   16 strings is the default. (This means 16 strings with the length of `ATRC_STACK_MAX_STRING_LENGTH`).
 * 
 * - `ATRC_STACK_MAX_BLOCKS`
 *   Define this to set the maximum number of blocks in a stack-allocated ATRC file descriptor.
 *   64 blocks is the default.
 * 
 * - `ATRC_STACK_MAX_VARIABLES`
 *   Define this to set the maximum number of variables in a stack-allocated ATRC file descriptor.
 *   64 variables is the default.
 * 
 * - `ATRC_STACK_MAX_KEYS`
 *   Define this to set the maximum number of keys in a block descriptor.
 *   64 keys is the default.
 * 
 * ### Export Definitions
 * 
 * Use these definitions when building the ATRC library as a shared or static library:
 * 
 * - `ATRC_EXPORTS`  
 *   Define this to export the ATRC API functions if building into a library.
 * 
 * - `ATRC_API`  
 *   Define this to specify function visibility for the ATRC API manually
 *   Defaults to `__declspec(dllexport)` on Windows and `__attribute__((visibility("default")))` on other platforms.
 * 
 * - `ATRC_DEVELOPMENT`  
 *   Define this to export additional developer/debug functions if building into a library. `ATRC_EXPORTS` must also be defined.
 *
 * - `ATRC_DEVELOPMENT_API`  
 *   Define this to specify visibility for the developer API functions manually
 *   Defaults to `__declspec(dllexport)` on Windows and `__attribute__((visibility("default")))` on other platforms.
 * 
 * ### Debugging Definitions
 * - `ATRC_DEBUG`  
 *   Enable debugging information and assertions.
 * 
 * - `ATRC_LOGGING`  
 *   Enable ATRC error logging output.
 * 
 * - `ATRC_NO_DEPRECATED`  
 *   Disable deprecated feature/function usage warnings.
 * 
 * - `ATRC_NO_WARNINGS`  
 *   Disable all warnings in the ATRC library.
 */
#pragma once
#ifndef ATRC_H
#define ATRC_H

// ATRC_API definition for cross-platform visibility
#ifndef ATRC_API
#   if defined(_WIN32) || defined(_WIN64)
#       ifdef ATRC_EXPORTS
#           define ATRC_API __declspec(dllexport)
#       else
#           define ATRC_API __declspec(dllimport)
#       endif
#       ifdef ATRC_DEVELOPMENT
#           ifndef ATRC_DEVELOPMENT_API
#               define ATRC_DEVELOPMENT_API __declspec(dllexport)
#           endif
#       endif
#   else
#       ifdef ATRC_EXPORTS
#           define ATRC_API __attribute__((visibility("default")))
#       else
#           define ATRC_API
#       endif
#       ifdef ATRC_DEVELOPMENT
#           ifndef ATRC_DEVELOPMENT_API
#               define ATRC_DEVELOPMENT_API __attribute__((visibility("default")))
#           endif
#       endif
#   endif
#endif

#ifndef ATRC_DEVELOPMENT_API
#define ATRC_DEVELOPMENT_API
#endif


#if !defined(ATRC_USE_STACK) && !defined(ATRC_USE_HEAP)
    // Default to stack mode if neither is defined
#   define ATRC_USE_STACK 1
#elif defined(ATRC_USE_STACK) && defined(ATRC_USE_HEAP)
#   error "Cannot define both ATRC_USE_STACK and ATRC_USE_HEAP"
#endif


#ifndef ATRC_STACK_MAX_STRING_LENGTH
#   define ATRC_STACK_MAX_STRING_LENGTH 128
#endif // ATRC_STACK_MAX_STRING_LENGTH

#ifndef ATRC_STACK_MAX_STRING_ARRAY_LENGTH
#   define ATRC_STACK_MAX_STRING_ARRAY_LENGTH 16
#endif // ATRC_STACK_MAX_STRING_ARRAY_LENGTH

#ifndef ATRC_STACK_MAX_BLOCKS
#   define ATRC_STACK_MAX_BLOCKS 64
#endif // ATRC_STACK_MAX_BLOCKS

#ifndef ATRC_STACK_MAX_VARIABLES
#   define ATRC_STACK_MAX_VARIABLES 64
#endif // ATRC_STACK_MAX_VARIABLES

#ifndef ATRC_STACK_MAX_KEYS
#   define ATRC_STACK_MAX_KEYS 64
#endif // ATRC_STACK_MAX_KEYS

#ifdef __cplusplus
#   include <cstdint>
extern "C" {
#else
#   include <stdint.h>
#   include <stdbool.h>
#endif

/// @brief Enumeration for read modes in ATRC
enum read_mode {
    /// @brief Reads only existing files
    ATRC_READ_ONLY = 0,

    /// @brief Reads and creates files if they do not exist
    ATRC_READ_CREATE,
    
    /// @brief Deletes existing file and creates new one
    ATRC_READ_FORCE,
};

/// @brief String array structure for ATRC
/// @details This structure is used to hold an array of strings
struct _STRING_ARRAY {
    /// @brief Pointer to the array of strings
#ifdef ATRC_USE_HEAP
    char** data;
#else
    char* data[ATRC_STACK_MAX_STRING_ARRAY_LENGTH];
#endif // ATRC_USE_HEAP
    /// @brief Number of strings in the array
    uint64_t count;
};

/// @brief String array type
typedef struct _STRING_ARRAY string_array_t;

/// @brief Read mode type
typedef enum _read_mode read_mode_t;

/// @brief Key structure for ATRC
struct _KEY {
    /// @brief Name of the key
#ifdef ATRC_USE_HEAP
    char* name;
#else
    char name[ATRC_STACK_MAX_STRING_LENGTH];
#endif // ATRC_USE_HEAP
    /// @brief Indicates if the key is a list
    /// @details If true, the value will be null and list_value will contain the list
    bool is_list;
    /// @brief Value of the key
    /// @details If the key is a list, this will be null.
#ifdef ATRC_USE_HEAP
    char* value;
#else
    char value[ATRC_STACK_MAX_STRING_LENGTH];
#endif // ATRC_USE_HEAP
    /// @brief List value of the key
    /// @details If the key is not a list, this will be NULL.
    string_array_t list_value;
    /// @brief Line number in the source file
    uint64_t line_number;
    /// @brief Enum value for the key
    uint64_t enum_value;
};

/// @brief Key-value pair type
typedef struct _KEY key_t;

/// @brief Block structure for ATRC
struct _BLOCK {
    /// @brief Name of the block
#ifdef ATRC_USE_HEAP
    char* name;
#else
    char name[ATRC_STACK_MAX_STRING_LENGTH];
#endif // ATRC_USE_HEAP

    /// @brief Line number in the source file
    uint64_t line_number;
    /// @brief Array of keys in the block
#ifdef ATRC_USE_HEAP
    key_t* keys;
#else
    key_t keys[ATRC_STACK_MAX_KEYS];
#endif // ATRC_USE_HEAP
    /// @brief Count of keys in the block
    uint64_t key_count;
};

/// @brief Block type
typedef struct _BLOCK block_t;

/// @brief Variable structure for ATRC
struct _VARIABLE {
    /// @brief Name of the variable
#ifdef ATRC_USE_HEAP
    char* name;
#else
    char name[ATRC_STACK_MAX_STRING_LENGTH];
#endif // ATRC_USE_HEAP
    /// @brief Indicates if the variable is a list
    /// @details If true, the value will be null and list_value will contain the list
    bool is_list;
    /// @brief Value of the variable.
    /// @details If the variable is a list, this will be null.
    /// @details If the variable is private, this will be NULL
#ifdef ATRC_USE_HEAP
    char* value;
#else
    char value[ATRC_STACK_MAX_STRING_LENGTH];
#endif // ATRC_USE_HEAP
    /// @brief List value of the variable.
    /// @details If the variable is not a list, this will be NULL.
    /// @details If the variable is private, this will be NULL
    string_array_t list_value;
    /// @brief Indicates if the variable is public
    bool is_public;
    /// @brief Line number in the source file
    uint64_t line_number;
};

/// @brief Variable type
typedef struct _VARIABLE variable_t;

/// @brief File descriptor structure for ATRC
/// @warning Do NOT modify this structure directly. Only use the provided functions to manipulate it.
struct _ATRC_FD {
    /// @brief File descriptor for the ATRC file
    /// @details This is used internally to manage the file operations
    int fd;
    /// @brief Path to the ATRC file
#ifdef ATRC_USE_HEAP
    char* path;
#else
    char path[ATRC_STACK_MAX_STRING_LENGTH];
#endif // ATRC_USE_HEAP
    /// @brief Read mode for the ATRC file
    read_mode_t mode;
    /// @brief Pointer to a list of variables
#ifdef ATRC_USE_HEAP
    variable_t* variables;
#else
    variable_t variables[ATRC_STACK_MAX_VARIABLES];
#endif // ATRC_USE_HEAP
    /// @brief Count of variables
    uint64_t variable_count;
    /// @brief Pointer to a list of blocks
#ifdef ATRC_USE_HEAP
    block_t* blocks;
#else
    block_t blocks[ATRC_STACK_MAX_BLOCKS];
#endif // ATRC_USE_HEAP
    /// @brief Count of blocks
    uint64_t block_count;
    /// @brief Indicates if the file descriptor is safe to use
    bool safe_to_use;
    /// @brief Indicates if auto-save is enabled
    bool auto_save;
    /// @brief Indicates if write check is enabled
    /// @details If true, keys and variables will be created when modifying them
    /// @details If true, block will be created for key if it does not exist
    bool write_check;
    /// @brief Indicates if the file is currently being read
    bool is_reading;
};

/// @brief File descriptor type for ATRC
/// @warning Do NOT modify this structure directly. Only use the provided functions to manipulate it.
typedef struct _ATRC_FD ATRC_FD;



/// @brief ATRC Return Values
enum _ATRC_RETVAL {
    /// @brief No error
    ATRC_ERROR_NONE = 0,
    /// @brief File not found or could not be opened
    ATRC_ERROR_FILE_NOT_FOUND,
    /// @brief Invalid read mode
    ATRC_ERROR_INVALID_MODE,
    /// @brief Read operation failed
    ATRC_ERROR_READ_FAILED,
    /// @brief Write operation failed
    ATRC_ERROR_WRITE_FAILED,
    /// @brief Invalid argument provided
    ATRC_ERROR_INVALID_ARGUMENT,
    /// @brief Memory allocation failed
    ATRC_ERROR_OUT_OF_MEMORY
};

/// @brief Return type for ATRC functions
typedef enum _ATRC_RETVAL atrc_retval_t;

// === PUBLIC API ===

/// @brief Initializes the ATRC file descriptor
/// @param fd Pointer to the ATRC file descriptor to initialize
ATRC_API atrc_retval_t init_atrc_fd(ATRC_FD* fd);

/// @brief Reads an ATRC file with the specified path and mode
/// @param fd Pointer to the ATRC file descriptor
/// @param path Path to the ATRC file to read
/// @param mode Read mode for the ATRC file
/// @return atrc_retval_t indicating the result of the operation
ATRC_API atrc_retval_t read_file_ex(ATRC_FD* fd, const char* path, read_mode_t mode);

/// @brief Reads an ATRC file with the specified path, in ATRC_READ_ONLY mode
/// @param fd Pointer to the ATRC file descriptor
/// @param path Path to the ATRC file to read
/// @return atrc_retval_t indicating the result of the operation
ATRC_API atrc_retval_t read_file(ATRC_FD* fd, const char* path);

/// @brief Reads an ATRC file again with the specified mode
/// @param fd Pointer to the ATRC file descriptor
/// @param mode Read mode for the ATRC file
/// @return atrc_retval_t indicating the result of the operation
ATRC_API atrc_retval_t read_file_again(ATRC_FD* fd, read_mode_t mode);

/// @brief Checks whether the ATRC file descriptor is in a valid state to use
/// @param fd Pointer to the ATRC file descriptor
/// @return True if the file descriptor is safe to use, false otherwise
ATRC_API bool check_status(ATRC_FD* fd);

/// @brief Frees the resources associated with the ATRC file descriptor
/// @details If stack in use, this will clear the stack buffers.
/// @param fd Pointer to the ATRC file descriptor to free
/// @return None
ATRC_API void free_atrc_fd(ATRC_FD* fd);
ATRC_API void free_blocks(ATRC_FD* fd);
ATRC_API void free_variables(ATRC_FD* fd);

ATRC_API char *read_variable(ATRC_FD* fd, const char* varname);
ATRC_API char *read_key(ATRC_FD* fd, const char* block, const char* key);
ATRC_API string_array_t* read_all_variables(ATRC_FD* fd);
ATRC_API string_array_t* read_all_keys(ATRC_FD* fd, const char* block);
ATRC_API string_array_t* read_key_list_value(ATRC_FD* fd, const char* block, const char* key);
ATRC_API string_array_t* read_variable_list_value(ATRC_FD* fd, const char* varname);
ATRC_API void destroy_string_array(string_array_t* array);
ATRC_API bool does_exist_block(ATRC_FD* fd, const char* block);
ATRC_API bool does_exist_variable(ATRC_FD* fd, const char* varname);
ATRC_API bool does_exist_key(ATRC_FD* fd, const char* block, const char* key);
ATRC_API bool is_public(ATRC_FD* fd, const char* varname);
ATRC_API void insert_var(ATRC_FD* fd, const char* line, const char** args, size_t arg_count);
ATRC_API char *insert_var_s(ATRC_FD* fd, const char* line, const char** args, size_t arg_count);
ATRC_API bool add_block(ATRC_FD* fd, const char* blockname);
ATRC_API bool remove_block(ATRC_FD* fd, const char* blockname);
ATRC_API bool add_variable(ATRC_FD* fd, const char* varname, const char* value);
ATRC_API bool remove_variable(ATRC_FD* fd, const char* varname);
ATRC_API bool modify_variable(ATRC_FD* fd, const char* varname, const char* value);
ATRC_API bool add_key(ATRC_FD* fd, const char* block, const char* key, const char* value);
ATRC_API bool remove_key(ATRC_FD* fd, const char* block, const char* key);
ATRC_API bool modify_key(ATRC_FD* fd, const char* block, const char* key, const char* value);
ATRC_API bool write_comment_to_top(ATRC_FD* fd, const char* comment);
ATRC_API bool write_comment_to_bottom(ATRC_FD* fd, const char* comment);
ATRC_API ATRC_FD* copy_atrc_fd(ATRC_FD* fd);
ATRC_API uint64_t get_enum_value(ATRC_FD* fd, const char* block, const char* key);
ATRC_API void set_auto_save(ATRC_FD* fd, bool auto_save);
ATRC_API void set_write_check(ATRC_FD* fd, bool write_check);
ATRC_API bool get_auto_save(ATRC_FD* fd);
ATRC_API bool get_write_check(ATRC_FD* fd);
ATRC_API key_t *get_variables(ATRC_FD* fd, uint64_t* count);
ATRC_API block_t *get_blocks(ATRC_FD* fd, uint64_t* count);


// === IMPLEMENTATION SECTION ===
#ifdef ATRC_IMPLEMENTATION
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#   ifdef ATRC_USE_HEAP
#       define __STRDUP(dest, src)                          \
            do {                                               \
                (dest) = _strdup(src);                      \
            } while (0)
#   else
#       define __STRDUP(dest, src)                          \
            do {                                               \
                strncpy((dest), (src), ATRC_STACK_MAX_STRING_LENGTH); \
                (dest)[ATRC_STACK_MAX_STRING_LENGTH - 1] = '\0';         \
            } while (0)
#   endif
#else
#   ifdef ATRC_USE_HEAP
#       define __STRDUP(dest, src)                          \
            do {                                               \
                (dest) = strdup(src);                       \
            } while (0)
#   else
#       define __STRDUP(dest, src)                          \
            do {                                               \
                strncpy((dest), (src), ATRC_STACK_MAX_STRING_LENGTH); \
                (dest)[ATRC_STACK_MAX_STRING_LENGTH - 1] = '\0';         \
            } while (0)
#   endif
#endif

#ifdef ATRC_USE_HEAP
#define __SET_NULL(dest) (dest) = NULL
#else
#define __SET_NULL(dest) memset((dest), 0, sizeof(*(dest)))
#endif

ATRC_DEVELOPMENT_API void __atrc_debug_log(const char* message, const char *str_data, int data) 
{
#ifdef ATRC_DEBUG
    fprintf(stderr, "~~ATRC Debug: %s (Data: %s, %d)\n", message, str_data, data);
#endif // ATRC_DEBUG
}


ATRC_DEVELOPMENT_API void __atrc_error_log(const char* message, int error_code) 
{
#ifdef ATRC_LOGGING
    fprintf(stderr, "~~ATRC Log: %s (Error Code: %d)\n", message, error_code);
#endif // ATRC_LOGGING
}

#define __ATRC_FILEDESCRIPTOR__ACTIVE__ 0xFF03
#define __MAX_UINT (uint64_t)-1
/*+++
Characters with special meaning in ATRC files:
- `#+++` : Comment block start
- `---#`: Comment block end
- `+`
- `-` 
- `#` : Denotes a comment line
- `.` : Denotes: #. -> Preprocessor directive, @block.key -> Reference to a key in a block
- `=` : Key-value separator
- `[` : Block name start or list start
- `]` : Block name end or list end
- `R` : If before `"`, it indicates a raw string, e.g., R"...".
- `"` : String start and end
- `@` : Variable start or reference to a variable, or @@, @0, @1, etc. for special variables
- `<` : If before variable creation, it indicates a variable is private, e.g., <@variable
- `,` : List item separator
- `\` : Escape character for special characters in strings e.g., `\"`, `\\`, \n \tetc.

Tokens are parsed into a linked list of `TOKEN` structs, which contain:
Types of tokens are:
- `TOKEN_UNKNOWN` : Unknown token type
- `TOKEN_EOF` : End of file token
- `TOKEN_COMMENT` : Comment token. Parsed from `#`
- `TOKEN_PLUS` : Plus token. Parsed from `+`
- `TOKEN_MINUS` : Minus token. Parsed from `-`
- `TOKEN_DOT` : Dot token. Parsed from `.`
- `TOKEN_EQUAL` : Equal token. Parsed from `=`
- `TOKEN_LBRACKET` : Left bracket token. Parsed from `[`
- `TOKEN_RBRACKET` : Right bracket token. Parsed from `]`
- `TOKEN_R` : Raw string token. Parsed from `R"`
- `TOKEN_QUOTE` : Quote token. Parsed from `"`
- `TOKEN_AT` : At token. Parsed from `@`
- `TOKEN_LESS` : Less than token. Parsed from `<`
- `TOKEN_COMMA` : Comma token. Parsed from `,`
- `TOKEN_BACKSLASH` : Backslash token. Parsed from `\`
---*/
enum __TOKEN_TYPE {
    TOKEN_UNKNOWN = 0,
    TOKEN_EOF,
    TOKEN_COMMENT,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_DOT,
    TOKEN_EQUAL,
    TOKEN_LBRACKET,
    TOKEN_RBRACKET,
    TOKEN_R,
    TOKEN_QUOTE,
    TOKEN_AT,
    TOKEN_LESS,
    TOKEN_COMMA,
    TOKEN_BACKSLASH,
    TOKEN_MASTER,
};
typedef enum __TOKEN_TYPE TOKEN_TYPE;

struct __TOKENIZER_TOKEN {
    TOKEN_TYPE type; // Token type
    char* value; // Token value
    uint64_t start_pos; // Start position in the source text
    uint64_t end_pos; // End position in the source text
    uint64_t line_number; // Line number in the source text
    uint64_t column_number; // Column number in the source text
    struct __TOKEN* next; // Pointer to the next token in the linked list
    struct __TOKEN* prev; // Pointer to the previous token in the linked list
};
typedef struct __TOKENIZER_TOKEN TOKEN;

typedef TOKEN TOKENIZER_MASTER;

enum __PARSE_TOKEN_TYPE {
    PARSE_TOKEN_UNKNOWN = 0,

    PARSE_TOKEN,
};
typedef enum __PARSE_TOKEN_TYPE PARSE_TOKEN_TYPE;

struct __PARSE_TOKEN {
    TOKEN token_type; // Type of the parse token
    char* token_value;
    PARSE_TOKEN_TYPE parse_type; // Type of the parse token
    char* parse_value; // Token value
    uint64_t start_pos; // Start position in the source text
    uint64_t end_pos; // End position in the source text
    uint64_t line_number; // Line number in the source text
    uint64_t column_number; // Column number in the source text
};
typedef struct __PARSE_TOKEN PARSE_TOKEN;

typedef struct __PARSE_TOKEN PARSE_MASTER_TOKEN;

ATRC_DEVELOPMENT_API atrc_retval_t __create_master_token(TOKENIZER_MASTER* master_token) {

}
ATRC_DEVELOPMENT_API void __destroy_master_token(TOKENIZER_MASTER* master_token) {

ATRC_DEVELOPMENT_API void __add_to_token(TOKEN* location, TOKEN* new_token) {

};
ATRC_DEVELOPMENT_API TOKEN* __create_token(TOKEN_TYPE type, const char* value, uint64_t start_pos, uint64_t end_pos, uint64_t line_number, uint64_t column_number) {

}

#define __LEVEL_INCREMENT__ 1
ATRC_DEVELOPMENT_API void __print_token(int level,const TOKEN* token) {

}

/// @brief Tokenizes a file and creates a master token
/// @param path Path to the file to tokenize
/// @param master_token Pointer to the master token to fill with the tokenized data
/// @return atrc_retval_t indicating the result of the tokenization
ATRC_DEVELOPMENT_API atrc_retval_t __tokenize_file(const char* path, TOKENIZER_MASTER* master_token) {

}

ATRC_DEVELOPMENT_API atrc_retval_t __create_parse_master_token(TOKENIZER_MASTER* master_token, PARSE_MASTER_TOKEN* parse_master_token);
ATRC_DEVELOPMENT_API void __destroy_parse_master_token(PARSE_MASTER_TOKEN* parse_master_token);

ATRC_DEVELOPMENT_API atrc_retval_t __parse_master_token_into_filedata(PARSE_MASTER_TOKEN* parse_master_token, ATRC_FD* fd);


atrc_retval_t init_atrc_fd(ATRC_FD* fd) {

}
atrc_retval_t read_file(ATRC_FD* fd, const char* path) {
    return read_file_ex(fd, path, ATRC_READ_ONLY);
}
atrc_retval_t read_file_ex(ATRC_FD* fd, const char* path, read_mode_t mode) {

}
atrc_retval_t read_file_again(ATRC_FD* fd, read_mode_t mode) {
    return read_file_ex(fd, fd->path, mode);
}

bool check_status(ATRC_FD* fd) {

}
void free_blocks(ATRC_FD* fd) {

}
void free_variables(ATRC_FD* fd) {

}
void free_atrc_fd(ATRC_FD* fd) {

}
#endif // ATRC_IMPLEMENTATION

#ifdef __cplusplus
} // extern "C"
#endif
#endif // ATRC_H
