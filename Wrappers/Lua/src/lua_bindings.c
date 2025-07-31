#include <ATRC.h>
#include "../libs/Lua/include/lua.h"
#include "../libs/Lua/include/lauxlib.h"
#include "../libs/Lua/include/lualib.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
  #define EXPORT __declspec(dllexport)
#else
  #define EXPORT
#endif


int Add(int x, int y) {
    return x + y;
}

int lua_Add(lua_State *L) {
    // Check and get two integer arguments from Lua stack
    int x = (int)luaL_checkinteger(L, 1); // 1st argument
    int y = (int)luaL_checkinteger(L, 2); // 2nd argument

    int result = Add(x, y);

    // Push the result on Lua stack
    lua_pushinteger(L, result);

    // Return 1 value
    return 1;
}

int lua_ReadATRC_FD(lua_State *L) {
    PATRC_FD fd = (PATRC_FD)luaL_checkudata(L, 1, "ATRC_FD");
    if (!fd) {
        return luaL_error(L, "Invalid ATRC_FD userdata");
    }
    const char *filename = luaL_checkstring(L, 2);
    bool res = Read(fd, filename, ATRC_READ_ONLY);
    if (!res) {
        return luaL_error(L, "Failed to read ATRC file: %s", filename);
    }
    lua_pushboolean(L, res);
    // Return 1 value (the boolean result)
    return 1;
}


int lua_Create_ATRC_FD(lua_State *L) {
    const char *filename = luaL_checkstring(L, 1);

    ATRC_FD **ud = (ATRC_FD **)lua_newuserdata(L, sizeof(ATRC_FD *));
    *ud = Create_ATRC_FD(filename, ATRC_READ_ONLY);
    if (!*ud) {
        return luaL_error(L, "Create_ATRC_FD failed for file: %s", filename);
    }

    // Set the metatable so Lua knows how to recognize and manage it
    luaL_getmetatable(L, "ATRC_FD");
    lua_setmetatable(L, -2);

    return 1;  // Return userdata object
}

int lua_Destroy_ATRC_FD(lua_State *L) {
    ATRC_FD **ud = (ATRC_FD **)luaL_checkudata(L, 1, "ATRC_FD");
    if (*ud) {
        Destroy_ATRC_FD(*ud);
        *ud = NULL;  // Prevent double-free
    }
    return 0;
}

int lua_ATRC_FD_gc(lua_State *L) {
    return lua_Destroy_ATRC_FD(L);
}

int lua_str_to_bool(lua_State *L) {
    const char *str = luaL_checkstring(L, 1);
    bool value = atrc_to_bool(str);
    lua_pushboolean(L, value);
    return 1;
}
int lua_atrc_to_int64_t(lua_State *L) {
    const char *str = luaL_checkstring(L, 1);
    int64_t value = atrc_to_int64_t(str);
    lua_pushinteger(L, value);
    return 1;
}
int lua_atrc_to_double(lua_State *L) {
    const char *str = luaL_checkstring(L, 1);
    double value = atrc_to_double(str);
    lua_pushnumber(L, value);
    return 1;
}
int lua_atrc_math_exp(lua_State *L) {
    const char *str = luaL_checkstring(L, 1);
    double value = atrc_math_exp(str);
    lua_pushnumber(L, value);
    return 1;
}


int lua_ReadVariable(lua_State *L) {
    ATRC_FD **ud = (ATRC_FD **)luaL_checkudata(L, 1, "ATRC_FD");
    if (!*ud) {
        return luaL_error(L, "Invalid ATRC_FD userdata");
    }
    const char *varname = luaL_checkstring(L, 2);
    char *value = ReadVariable(*ud, varname);
    if (!value) {
        lua_pushstring(L, NULL);  // Push nil if variable not found
        return 1;  // Return nil
    }
    lua_pushstring(L, value);
    ATRC_FREE_MEMORY(value);  // Free the memory allocated by ReadVariable
    return 1;  // Return the variable value as a string
}
int lua_ReadKey(lua_State *L) {
    ATRC_FD **ud = (ATRC_FD **)luaL_checkudata(L, 1, "ATRC_FD");
    if (!*ud) {
        return luaL_error(L, "Invalid ATRC_FD userdata");
    }
    const char *block = luaL_checkstring(L, 2);
    const char *key = luaL_checkstring(L, 3);

    char *value = ReadKey(*ud, block, key);
    if (!value) {
        lua_pushstring(L, NULL);  // Push nil if key not found
        return 1;  // Return nil
    }
    lua_pushstring(L, value);
    ATRC_FREE_MEMORY(value);  // Free the memory allocated by ReadKey
    return 1;  // Return the key value as a string
}

int lua_DoesExistBlock(lua_State *L) {
    ATRC_FD **ud = (ATRC_FD **)luaL_checkudata(L, 1, "ATRC_FD");
    if (!*ud) {
        return luaL_error(L, "Invalid ATRC_FD userdata");
    }
    const char *block = luaL_checkstring(L, 2);
    if(!block) {
        return luaL_error(L, "Block name cannot be NULL");
    }
    bool exists = DoesExistBlock(*ud, block);
    lua_pushboolean(L, exists);
    return 1;  // Return boolean result
}
int lua_DoesExistVariable(lua_State *L) {
    ATRC_FD **ud = (ATRC_FD **)luaL_checkudata(L, 1, "ATRC_FD");
    if (!*ud) {
        return luaL_error(L, "Invalid ATRC_FD userdata");
    }
    const char *varname = luaL_checkstring(L, 2);
    bool exists = DoesExistVariable(*ud, varname);
    lua_pushboolean(L, exists);
    return 1;  // Return boolean result
}
int lua_DoesExistKey(lua_State *L) {
    ATRC_FD **ud = (ATRC_FD **)luaL_checkudata(L, 1, "ATRC_FD");
    if (!*ud) {
        return luaL_error(L, "Invalid ATRC_FD userdata");
    }
    const char *block = luaL_checkstring(L, 2);
    if(!block) {
        return luaL_error(L, "Block name cannot be NULL");
    }
    const char *key = luaL_checkstring(L, 3);
    if(!key) {
        return luaL_error(L, "Key name cannot be NULL");
    }
    bool exists = DoesExistKey(*ud, block, key);
    lua_pushboolean(L, exists);
    return 1;  // Return boolean result
}

int lua_IsPublic(lua_State *L) {
    ATRC_FD **ud = (ATRC_FD **)luaL_checkudata(L, 1, "ATRC_FD");
    if (!*ud) {
        return luaL_error(L, "Invalid ATRC_FD userdata");
    }
    const char *varname = luaL_checkstring(L, 2);
    bool isPublic = IsPublic(*ud, varname);
    lua_pushboolean(L, isPublic);
    return 1;  // Return boolean result
}

int lua_InsertVars(lua_State *L) {
    const char *line = luaL_checkstring(L, 1);
    luaL_checktype(L, 2, LUA_TTABLE);  // Ensure second argument
    // is a table
    // Get the number of elements in the table
    lua_len(L, 2);  // Push the length of the table onto the stack
    int n = lua_tointeger(L, -1);  // Get the length
    lua_pop(L, 1);  // Pop the length from the stack
    const char **args = (const char **)malloc((n + 1) * sizeof(const char *));
    if (!args) {
        return luaL_error(L, "Memory allocation failed for args");
    }
    for (int i = 0; i < n; i++) {
        lua_pushinteger(L, i + 1);  // Lua indices start at 1
        lua_gettable(L, 2);  // Get the value at index i+1
        args[i] = luaL_checkstring(L, -1);  // Check and get the string
        lua_pop(L, 1);  // Pop the value from the stack
    }
    args[n] = NULL;  // Null-terminate the array of strings
    char *result = InsertVar_S(line, args);
    free(args);  // Free the allocated memory for args
    if (!result) {
        return luaL_error(L, "InsertVar_S failed for line: %s", line);
    }
    lua_pushstring(L, result);  // Push the result string onto the stack
    ATRC_FREE_MEMORY(result);  // Free the memory allocated by InsertVar_S
    return 1;  // Return the result string
}

int lua_AddBlock(lua_State *L) {
    ATRC_FD **ud = (ATRC_FD **)luaL_checkudata(L, 1, "ATRC_FD");
    if (!*ud) {
        return luaL_error(L, "Invalid ATRC_FD userdata");
    }
    const char *block = luaL_checkstring(L, 2);
    if (!block) {
        return luaL_error(L, "Block name cannot be NULL");
    }
    bool success = AddBlock(*ud, block);
    lua_pushboolean(L, success);
    return 1;  // Return boolean result
}
int lua_RemoveBlock(lua_State *L) {
    ATRC_FD **ud = (ATRC_FD **)luaL_checkudata(L, 1, "ATRC_FD");
    if (!*ud) {
        return luaL_error(L, "Invalid ATRC_FD userdata");
    }
    const char *block = luaL_checkstring(L, 2);
    if (!block) {
        return luaL_error(L, "Block name cannot be NULL");
    }
    bool success = RemoveBlock(*ud, block);
    lua_pushboolean(L, success);
    return 1;  // Return boolean result
}
int lua_AddVariable(lua_State *L) {
    ATRC_FD **ud = (ATRC_FD **)luaL_checkudata(L, 1, "ATRC_FD");
    if (!*ud) {
        return luaL_error(L, "Invalid ATRC_FD userdata");
    }
    const char *varname = luaL_checkstring(L, 2);
    const char *value = luaL_checkstring(L, 3);
    if (!varname || !value) {
        return luaL_error(L, "Variable name and value cannot be NULL");
    }
    bool success = AddVariable(*ud, varname, value);
    lua_pushboolean(L, success);
    return 1;  // Return boolean result
}
int lua_RemoveVariable(lua_State *L) {
    ATRC_FD **ud = (ATRC_FD **)luaL_checkudata(L, 1, "ATRC_FD");
    if (!*ud) {
        return luaL_error(L, "Invalid ATRC_FD userdata");
    }
    const char *varname = luaL_checkstring(L, 2);
    if (!varname) {
        return luaL_error(L, "Variable name cannot be NULL");
    }
    bool success = RemoveVariable(*ud, varname);
    lua_pushboolean(L, success);
    return 1;  // Return boolean result
}
int lua_ModifyVariable(lua_State *L) {
    ATRC_FD **ud = (ATRC_FD **)luaL_checkudata(L, 1, "ATRC_FD");
    if (!*ud) {
        return luaL_error(L, "Invalid ATRC_FD userdata");
    }
    const char *varname = luaL_checkstring(L, 2);
    const char *value = luaL_checkstring(L, 3);
    if (!varname || !value) {
        return luaL_error(L, "Variable name and value cannot be NULL");
    }
    bool success = ModifyVariable(*ud, varname, value);
    lua_pushboolean(L, success);
    return 1;  // Return boolean result
}
int lua_AddKey(lua_State *L) {
    ATRC_FD **ud = (ATRC_FD **)luaL_checkudata(L, 1, "ATRC_FD");
    if (!*ud) {
        return luaL_error(L, "Invalid ATRC_FD userdata");
    }
    const char *block = luaL_checkstring(L, 2);
    const char *key = luaL_checkstring(L, 3);
    const char *value = luaL_checkstring(L, 4);
    if (!block || !key || !value) {
        return luaL_error(L, "Block name, key and value cannot be NULL");
    }
    bool success = AddKey(*ud, block, key, value);
    lua_pushboolean(L, success);
    return 1;  // Return boolean result
}
int lua_RemoveKey(lua_State *L) {
    ATRC_FD **ud = (ATRC_FD **)luaL_checkudata(L, 1, "ATRC_FD");
    if (!*ud) {
        return luaL_error(L, "Invalid ATRC_FD userdata");
    }
    const char *block = luaL_checkstring(L, 2);
    const char *key = luaL_checkstring(L, 3);
    if (!block || !key) {
        return luaL_error(L, "Block name and key cannot be NULL");
    }
    bool success = RemoveKey(*ud, block, key);
    lua_pushboolean(L, success);
    return 1;  // Return boolean result
}
int lua_ModifyKey(lua_State *L) {
    ATRC_FD **ud = (ATRC_FD **)luaL_checkudata(L, 1, "ATRC_FD");
    if (!*ud) {
        return luaL_error(L, "Invalid ATRC_FD userdata");
    }
    const char *block = luaL_checkstring(L, 2);
    const char *key = luaL_checkstring(L, 3);
    const char *value = luaL_checkstring(L, 4);
    if (!block || !key || !value) {
        return luaL_error(L, "Block name, key and value cannot be NULL");
    }
    bool success = ModifyKey(*ud, block, key, value);
    lua_pushboolean(L, success);
    return 1;  // Return boolean result
}
int lua_WriteCommentToBottom(lua_State *L) {
    ATRC_FD **ud = (ATRC_FD **)luaL_checkudata(L, 1, "ATRC_FD");
    if (!*ud) {
        return luaL_error(L, "Invalid ATRC_FD userdata");
    }
    const char *comment = luaL_checkstring(L, 2);
    if (!comment) {
        return luaL_error(L, "Comment cannot be NULL");
    }
    bool success = WriteCommentToBottom(*ud, comment);
    lua_pushboolean(L, success);
    return 1;  // Return boolean result
}
int lua_WriteCommentToTop(lua_State *L) {
    ATRC_FD **ud = (ATRC_FD **)luaL_checkudata(L, 1, "ATRC_FD");
    if (!*ud) {
        return luaL_error(L, "Invalid ATRC_FD userdata");
    }
    const char *comment = luaL_checkstring(L, 2);
    if (!comment) {
        return luaL_error(L, "Comment cannot be NULL");
    }
    bool success = WriteCommentToTop(*ud, comment);
    lua_pushboolean(L, success);
    return 1;  // Return boolean result
}

int lua_GetEnumValue(lua_State *L) {
    ATRC_FD **ud = (ATRC_FD **)luaL_checkudata(L, 1, "ATRC_FD");
    if (!*ud) {
        return luaL_error(L, "Invalid ATRC_FD userdata");
    }
    const char *block = luaL_checkstring(L, 2);
    const char *key = luaL_checkstring(L, 3);
    if (!block || !key) {
        return luaL_error(L, "Block name and key cannot be NULL");
    }
    uint64_t value = GetEnumValue(*ud, block, key);
    lua_pushinteger(L, value);
    return 1;  // Return the enum value as an integer
}

int lua_SetAutoSave(lua_State *L){
    ATRC_FD **ud = (ATRC_FD **)luaL_checkudata(L, 1, "ATRC_FD");
    if (!*ud) {
        return luaL_error(L, "Invalid ATRC_FD userdata");
    }
    bool autoSave = lua_toboolean(L, 2);
    (*ud)->AutoSave = autoSave;
    lua_pushboolean(L, true);  // Return true to indicate success
    return 1;  // Return boolean result
}
int lua_GetAutoSave(lua_State *L){
    ATRC_FD **ud = (ATRC_FD **)luaL_checkudata(L, 1, "ATRC_FD");
    if (!*ud) {
        return luaL_error(L, "Invalid ATRC_FD userdata");
    }
    lua_pushboolean(L, (*ud)->AutoSave);
    return 1;  // Return the AutoSave value
}
int lua_GetFilename(lua_State *L){
    ATRC_FD **ud = (ATRC_FD **)luaL_checkudata(L, 1, "ATRC_FD");
    if (!*ud) {
        return luaL_error(L, "Invalid ATRC_FD userdata");
    }
    if ((*ud)->Filename) {
        lua_pushstring(L, (*ud)->Filename);
    } else {
        lua_pushnil(L);  // If Filename is NULL, push nil
    }
    return 1;  // Return the filename or nil
}

int lua_GetVariableCount(lua_State *L){
    ATRC_FD **ud = (ATRC_FD **)luaL_checkudata(L, 1, "ATRC_FD");
    if (!*ud) {
        return luaL_error(L, "Invalid ATRC_FD userdata");
    }
    lua_pushinteger(L, (*ud)->VariableArray.VariableCount);
    return 1;  // Return the variable count as an integer
}

int lua_SetWriteCheck(lua_State *L){
    ATRC_FD **ud = (ATRC_FD **)luaL_checkudata(L, 1, "ATRC_FD");
    if (!*ud) {
        return luaL_error(L, "Invalid ATRC_FD userdata");
    }
    bool writeCheck = lua_toboolean(L, 2);
    (*ud)->Writecheck = writeCheck;
    lua_pushboolean(L, true);  // Return true to indicate success
    return 1;  // Return boolean result
}
int lua_GetWriteCheck(lua_State *L){
    ATRC_FD **ud = (ATRC_FD **)luaL_checkudata(L, 1, "ATRC_FD");
    if (!*ud) {
        return luaL_error(L, "Invalid ATRC_FD userdata");
    }
    lua_pushboolean(L, (*ud)->Writecheck);
    return 1;  // Return the WriteCheck value
}

int lua_CopyATRC(lua_State *L) {
    ATRC_FD **ud = (ATRC_FD **)luaL_checkudata(L, 1, "ATRC_FD");
    if (!*ud) {
        return luaL_error(L, "Invalid ATRC_FD userdata");
    }

    PATRC_FD new_fd = Copy_ATRC_FD(*ud);
    if (!new_fd) {
        return luaL_error(L, "Copy_ATRC_FD failed");
    }

    // Create new userdata to hold the copied ATRC_FD pointer
    ATRC_FD **new_ud = (ATRC_FD **)lua_newuserdata(L, sizeof(ATRC_FD *));
    *new_ud = new_fd;

    // Set the metatable so Lua knows it's an ATRC_FD userdata
    luaL_getmetatable(L, "ATRC_FD");
    lua_setmetatable(L, -2);

    // The new userdata is now on the stack, return it
    return 1;
}



EXPORT int luaopen_ATRCLua(lua_State *L) {
    // Set up metatable for ATRC_FD
    if (luaL_newmetatable(L, "ATRC_FD")) {
        lua_pushcfunction(L, lua_Destroy_ATRC_FD);
        lua_setfield(L, -2, "__gc");
        lua_pop(L, 1);
    }

    static const struct luaL_Reg mylib_funcs[] = {
        // Test function
        {"Add", lua_Add},

        // ATRC_FD management functions
        {"OpenATRC", lua_Create_ATRC_FD},
        {"CloseATRC", lua_Destroy_ATRC_FD},
        {"ReadATRC", lua_ReadATRC_FD},
        {"SetAutosave", lua_SetAutoSave},
        {"GetAutosave", lua_GetAutoSave},
        {"GetFilename", lua_GetFilename},
        {"GetVariableCount", lua_GetVariableCount},
        {"SetWriteCheck", lua_SetWriteCheck},
        {"GetWriteCheck", lua_GetWriteCheck},

        {"ReadVariable", lua_ReadVariable},
        {"ReadKey", lua_ReadKey},
        {"DoesExistBlock", lua_DoesExistBlock},
        {"DoesExistVariable", lua_DoesExistVariable},
        {"DoesExistKey", lua_DoesExistKey},
        {"IsPublic", lua_IsPublic},
        {"InsertVars", lua_InsertVars},
        {"AddBlock", lua_AddBlock},
        {"RemoveBlock", lua_RemoveBlock},
        {"AddVariable", lua_AddVariable},
        {"RemoveVariable", lua_RemoveVariable},
        {"ModifyVariable", lua_ModifyVariable},
        {"AddKey", lua_AddKey},
        {"RemoveKey", lua_RemoveKey},
        {"ModifyKey", lua_ModifyKey},
        {"WriteCommentToBottom", lua_WriteCommentToBottom},
        {"WriteCommentToTop", lua_WriteCommentToTop},
        {"GetEnumValue", lua_GetEnumValue},
        {"CopyATRC", lua_CopyATRC},  // Alias for Create_ATRC_FD

        // Standard library functions
        {"StrToBool", lua_str_to_bool},
        {"StrToNum", lua_atrc_to_int64_t},
        {"StrToDouble", lua_atrc_to_double},
        {"ParseMathExp", lua_atrc_math_exp},
        {NULL, NULL}
    };

    luaL_newlib(L, mylib_funcs);
    return 1;
}

