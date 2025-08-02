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
#pragma once
#ifndef ATRC_H
#define ATRC_H

// ATRC_API definition for cross-platform visibility
#if defined(__clang__) // Clang
#   ifdef ATRC_EXPORTS
#       define ATRC_API __attribute__((visibility("default")))
#   else
#       define ATRC_API
#   endif

#elif defined(_MSC_VER) || defined(_WIN32) // MSVC or Windows
#   ifdef ATRC_EXPORTS
#       define ATRC_API __declspec(dllexport)
#   else
#       define ATRC_API __declspec(dllimport)
#   endif

#elif defined(__GNUC__) || defined(__GNUG__) // GCC
#   ifdef ATRC_EXPORTS
#       define ATRC_API __attribute__((visibility("default")))
#   else
#       define ATRC_API
#   endif

#else
#   define ATRC_API
#   pragma message("ATRC_API not defined for this compiler, using default visibility")
#endif

#ifdef ATRC_STATIC
#   define ATRC_API
#   pragma message("ATRC_API not defined for static build, using default visibility")
#endif

#define FILEHEADER ""
#define __atrc__one__of__two__(cpp, c) \
    __atrc__one__of__two_helper__(cpp, c)
#define __nmsp__    namespace atrc {
#define __nmspe__   }
#ifdef __cplusplus
    #define __atrc__one__of__two_helper__(cpp, c) cpp
#else
    #define __atrc__one__of__two_helper__(cpp, c) c
#endif

#ifdef __cplusplus
#   include <cstdint>
#else
#   include <stdint.h>
#   include <stdbool.h>
#endif // __cplusplus

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef enum _ReadMode {
    ATRC_READ_ONLY,
    ATRC_CREATE_READ,
    ATRC_FORCE_READ,
} ReadMode;


typedef struct _Variable {
    char *Name;
    char *Value;
    bool IsPublic;
    uint64_t line_number;
} Variable, *PVariable;

typedef struct _Variable_Arr {
    Variable *Variables;
    uint64_t VariableCount;
} Variable_Arr, *PVariable_Arr;



typedef struct _Key {
    char *Name;
    char *Value;
    uint64_t line_number;
    uint64_t enum_value;
} Key, *PKey;

typedef struct _Key_Arr {
    Key *Keys;
    uint64_t KeyCount;
} Key_Arr, *PKey_Arr;



typedef struct _Block {
    char *Name;
    Key_Arr KeyArray;
    uint64_t line_number;
} Block, *PBlock;

typedef struct _Block_Arr {
    Block *Blocks;
    uint64_t BlockCount;
} Block_Arr, *PBlock_Arr;



typedef struct _ATRCFiledata{
    Variable_Arr VariableArray;
    Block_Arr BlockArray;
    char *Filename;
    bool AutoSave;
    bool Writecheck;
} ATRC_FD, *PATRC_FD;

ATRC_API PATRC_FD Create_Empty_ATRC_FD();
#ifndef __CREATE_EMPTY_ATRC_FD
#   define __CREATE_EMPTY_ATRC_FD() \
        Create_Empty_ATRC_FD()
#endif // __CREATE_EMPTY_ATRC_FD

ATRC_API PATRC_FD Create_ATRC_FD(const char *filename, ReadMode readMode);
#ifndef __CREATE_ATRC_FD
#   define __CREATE_ATRC_FD(filename, readMode) \
        Create_ATRC_FD(filename, readMode)
#endif // __CREATE_ATRC_FD

ATRC_API void Destroy_ATRC_FD(PATRC_FD fd);
#ifndef __DESTROY_ATRC_FD
#   define __DESTROY_ATRC_FD() \
        Destroy_ATRC_FD(GLB_ATRC_FD)
#endif // __DESTROY_ATRC_FD

ATRC_API bool Read(PATRC_FD fd, const char* path, ReadMode readMode);
#ifndef __READ
#   define __READ(path, readMode) \
        Read(GLB_ATRC_FD, path, readMode)
#endif // __READ
    
ATRC_API char* ReadVariable(PATRC_FD fd, const char* varname);
#ifndef __READ_VAR
#   define __READ_VAR(varname) \
        ReadVariable(GLB_ATRC_FD, varname)
#endif // __READ_VAR

ATRC_API char* ReadKey(PATRC_FD fd, const char* block, const char* key);
#ifndef __READ_KEY
#   define __READ_KEY(block, key) \
        ReadKey(GLB_ATRC_FD, block, key)
#endif // __READ_KEY
    
ATRC_API bool DoesExistBlock(PATRC_FD fd, const char* block);
#ifndef __DOES_EXIST_BLOCK
#   define __DOES_EXIST_BLOCK(block) \
        DoesExistBlock(GLB_ATRC_FD, block)
#endif // __DOES_EXIST_BLOCK

ATRC_API bool DoesExistVariable(PATRC_FD fd, const char* varname);
#ifndef __DOES_EXIST_VAR
#   define __DOES_EXIST_VAR(varname) \
        DoesExistVariable(GLB_ATRC_FD, varname)
#endif // __DOES_EXIST_VAR

ATRC_API bool DoesExistKey(PATRC_FD fd, const char* block, const char* key);
#ifndef __DOES_EXIST_KEY
#   define __DOES_EXIST_KEY(block, key) \
        DoesExistKey(GLB_ATRC_FD, block, key)
#endif // __DOES_EXIST_KEY

ATRC_API bool IsPublic(PATRC_FD fd, const char* varname);
#ifndef __IS_PUBLIC
#   define __IS_PUBLIC(varname) \
        IsPublic(GLB_ATRC_FD, varname)
#endif // __IS_PUBLIC

ATRC_API char* InsertVar_S(const char* line, const char** args);
#ifndef __INSERT_VAR_S
#   define __INSERT_VAR_S(line, args) \
        InsertVar_S(line, args)
#endif // __INSERT_VAR_S

ATRC_API bool AddBlock(PATRC_FD fd, const char* blockname);
#ifndef __ADD_BLOCK
#   define __ADD_BLOCK(blockname) \
        AddBlock(GLB_ATRC_FD, blockname)
#endif // __ADD_BLOCK

ATRC_API bool RemoveBlock(PATRC_FD fd, const char* blockname);
#ifndef __REMOVE_BLOCK
#   define __REMOVE_BLOCK(blockname) \
        RemoveBlock(GLB_ATRC_FD, blockname)
#endif // __REMOVE_BLOCK

ATRC_API bool AddVariable(PATRC_FD fd, const char* varname, const char* value);
#ifndef __ADD_VAR
#   define __ADD_VAR(varname, value) \
        AddVariable(GLB_ATRC_FD, varname, value)
#endif // __ADD_VAR

ATRC_API bool RemoveVariable(PATRC_FD fd, const char* varname);
#ifndef __REMOVE_VAR
#   define __REMOVE_VAR(varname) \
        RemoveVariable(GLB_ATRC_FD, varname)
#endif // __REMOVE_VAR

ATRC_API bool ModifyVariable(PATRC_FD fd, const char* varname, const char* value);
#ifndef __MODIFY_VAR
#   define __MODIFY_VAR(varname, value) \
        ModifyVariable(GLB_ATRC_FD, varname, value)
#endif // __MODIFY_VAR

ATRC_API bool AddKey(PATRC_FD fd, const char* block, const char* key, const char* value);
#ifndef __ADD_KEY
#   define __ADD_KEY(block, key, value) \
        AddKey(GLB_ATRC_FD, block, key, value)
#endif // __ADD_KEY

ATRC_API bool RemoveKey(PATRC_FD fd, const char* block, const char* key);
#ifndef __REMOVE_KEY
#   define __REMOVE_KEY(block, key) \
        RemoveKey(GLB_ATRC_FD, block, key)
#endif // __REMOVE_KEY

ATRC_API bool ModifyKey(PATRC_FD fd, const char* block, const char* key, const char* value);
#ifndef __MODIFY_KEY
#   define __MODIFY_KEY(block, key, value) \
        ModifyKey(GLB_ATRC_FD, block, key, value)
#endif // __MODIFY_KEY
    
ATRC_API bool WriteCommentToBottom(PATRC_FD fd, const char* comment);
#ifndef __WRITE_COMMENT_TO_BOTTOM
#   define __WRITE_COMMENT_TO_BOTTOM(comment) \
        WriteCommentToBottom(GLB_ATRC_FD, comment)
#endif // __WRITE_COMMENT_TO_BOTTOM

ATRC_API bool WriteCommentToTop(PATRC_FD fd, const char* comment);
#ifndef __WRITE_COMMENT_TO_TOP
#   define __WRITE_COMMENT_TO_TOP(comment) \
        WriteCommentToTop(GLB_ATRC_FD, comment)
#endif // __WRITE_COMMENT_TO_TOP

ATRC_API uint64_t GetEnumValue(PATRC_FD fd, const char* block, const char* key);
#ifndef __GET_ENUM_VALUE
#   define __GET_ENUM_VALUE(block, key) \
        GetEnumValue(GLB_ATRC_FD, block, key)
#endif // __GET_ENUM_VALUE

ATRC_API void ATRC_FREE_MEMORY(void *ptr);
#ifndef __ATRC_FREE_MEMORY
#   define __ATRC_FREE_MEMORY(ptr) \
        ATRC_FREE_MEMORY(ptr)
#endif // __ATRC_FREE_MEMORY

#ifndef __ATRC_FREE_MEMORY_EX
#   define __ATRC_FREE_MEMORY_EX(ptr) \
        ATRC_FREE_MEMORY(ptr); \
        ptr = NULL;
#endif

ATRC_API PATRC_FD Copy_ATRC_FD(PATRC_FD fd);
#ifndef __COPY_ATRC_FD
#   define __COPY_ATRC_FD(fd) \
        Copy_ATRC_FD(fd)
#endif // __COPY_ATRC_FD

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus




/*+++
Start of ATRC standard library
---*/

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
/*
Inject macro for C
USAGE:
{
    const char *res = NULL;
    ATRC_INJECT("INPUT %*%, %*%\n", "arg1", "arg2", ...) // -> "INPUT arg1, arg2\n"
    // use res as needed
    ATRC_FREE_MEMORY(res);
    res = NULL;
}
*/
#ifndef ATRC_INJECT
#define ATRC_INJECT(line, ...) \
if(line != NULL) { \
    const char *message = line; \
    const char *args[] = {__VA_ARGS__, NULL}; \
    res = InsertVar_S(message, args); \
} \
else { \
    res = NULL; \
}
#endif // ATRC_INJECT

enum ATRC_ERR {
	_ATRC_SUCCESSFULL_ACTION = 0,
	_ATRC_UNSUCCESSFULL_ACTION = 1
};

extern uint64_t atrc_stdlib_errval;

typedef struct _String_Arr {
    char **list;
    uint64_t count;
} String_Arr, *PString_Arr;

ATRC_API PString_Arr atrc_to_list(const char separator, const char* value);
#ifndef __ATRC_TO_LIST
#   define __ATRC_TO_LIST(separator, value) \
        atrc_to_list(separator, value)
#endif // __ATRC_TO_LIST

ATRC_API void *atrc_free_list(PString_Arr list);
#ifndef __ATRC_FREE_LIST
#   define __ATRC_FREE_LIST(list) \
        atrc_free_list(list)
#endif // __ATRC_FREE_LIST

ATRC_API bool atrc_to_bool(const char* value);
#ifndef __ATRC_TO_BOOL
#   define __ATRC_TO_BOOL(value) \
        atrc_to_bool(value)
#endif // __ATRC_TO_BOOL

ATRC_API uint64_t atrc_to_uint64_t(const char* value);
#ifndef __ATRC_TO_UINT64_T
#   define __ATRC_TO_UINT64_T(value) \
        atrc_to_uint64_t(value)
#endif // __ATRC_TO_UINT64_T

ATRC_API int64_t atrc_to_int64_t(const char* value);
#ifndef __ATRC_TO_INT64_T
#   define __ATRC_TO_INT64_T(value) \
        atrc_to_int64_t(value)
#endif // __ATRC_TO_INT64_T

ATRC_API double atrc_to_double(const char* value);
#ifndef __ATRC_TO_DOUBLE
#   define __ATRC_TO_DOUBLE(value) \
        atrc_to_double(value)
#endif // __ATRC_TO_DOUBLE

ATRC_API double atrc_math_exp(const char* value);
#ifndef __ATRC_MATH_EXP
#   define __ATRC_MATH_EXP(value) \
        atrc_math_exp(value)
#endif // __ATRC_MATH_EXP

#ifdef __cplusplus
} // Extern "C"
#endif // __cplusplus

#endif // ATRC_H
