# ATRC C API Documentation

## Table of Contents

- [Overview](#overview)
- [Enums](#enums)
  - [ReadMode](#readmode)
- [Data Structures](#data-structures)
  - [Variable](#variable)
  - [Variable_Arr](#variable_arr)
  - [Key](#key)
  - [Key_Arr](#key_arr)
  - [Block](#block)
  - [Block_Arr](#block_arr)
  - [ATRC_FD](#atrc_fd)
- [Constructor/Destructor Functions](#constructordestructor-functions)
  - [Create_Empty_ATRC_FD](#create_empty_atrc_fd)
  - [Create_ATRC_FD](#create_atrc_fd)
  - [Destroy_ATRC_FD](#destroy_atrc_fd)
- [File Operations](#file-operations)
  - [Read](#read)
- [Data Access Functions](#data-access-functions)
  - [ReadVariable](#readvariable)
  - [ReadKey](#readkey)
- [Existence Check Functions](#existence-check-functions)
  - [DoesExistBlock](#doesexistblock)
  - [DoesExistVariable](#doesexistvariable)
  - [DoesExistKey](#doesexistkey)
  - [IsPublic](#ispublic)
- [Variable Injection](#variable-injection)
  - [InsertVar_S](#insertvar_s)
- [Block Management](#block-management)
  - [AddBlock](#addblock)
  - [RemoveBlock](#removeblock)
- [Variable Management](#variable-management)
  - [AddVariable](#addvariable)
  - [RemoveVariable](#removevariable)
  - [ModifyVariable](#modifyvariable)
- [Key Management](#key-management)
  - [AddKey](#addkey)
  - [RemoveKey](#removekey)
  - [ModifyKey](#modifykey)
- [Comment Functions](#comment-functions)
  - [WriteCommentToBottom](#writecommenttobottom)
  - [WriteCommentToTop](#writecommenttotop)
- [Enum Functions](#enum-functions)
  - [GetEnumValue](#getenumvalue)
- [Memory Management](#memory-management)
  - [ATRC_FREE_MEMORY](#atrc_free_memory)
- [Macro usage](#macro-usage)
- [Complete example](#complete-example)
---

## Overview

The ATRC C API provides a comprehensive interface for reading, writing, and manipulating ATRC configuration files. It supports variables, blocks, keys, comments, and advanced features like variable injection and enumeration values.

---

## Enums

### ReadMode

**Definition:**
```c
typedef enum _ReadMode {
    ATRC_READ_ONLY,    // Read file without creating if it doesn't exist
    ATRC_CREATE_READ,  // Create file if it doesn't exist, then read
    ATRC_FORCE_READ,   // Force read regardless of file state
} ReadMode;
```

**Remarks:**
- `ATRC_READ_ONLY`: Safest option for reading existing files
- `ATRC_CREATE_READ`: If file does not exist, new one is created and read.
- `ATRC_FORCE_READ`: Deletes file if it exists, creates new one and reads it.

**Example:**
```c
ReadMode mode = ATRC_READ_ONLY;
PATRC_FD fd = Create_ATRC_FD("config.atrc", mode);
```

---

## Data Structures

### Variable

**Definition:**
```c
typedef struct _Variable {
    char *Name;           // Variable name
    char *Value;          // Variable value
    bool IsPublic;        // Whether variable is public
    uint64_t line_number; // Line number in file
} Variable, *PVariable;
```

**Remarks:**
- Variables are global within an ATRC file
- Public variables can be read outside the file
- Line number helps with debugging and error reporting

### Variable_Arr

**Definition:**
```c
typedef struct _Variable_Arr {
    Variable *Variables;    // Array of variables
    uint64_t VariableCount; // Number of variables
} Variable_Arr, *PVariable_Arr;
```

**Remarks:**
- Container for managing multiple variables
- Provides count for safe iteration

### Key

**Definition:**
```c
typedef struct _Key {
    char *Name;           // Key name
    char *Value;          // Key value
    uint64_t line_number; // Line number in file
    uint64_t enum_value;  // Enumeration value for this key
} Key, *PKey;
```

**Remarks:**
- Keys exist within blocks
- Enum value is automatically assigned based on order of appearance
- First key in a block gets enum_value = 0

**Example:**

### Key_Arr

**Definition:**
```c
typedef struct _Key_Arr {
    Key *Keys;         // Array of keys
    uint64_t KeyCount; // Number of keys
} Key_Arr, *PKey_Arr;
```

### Block

**Definition:**
```c
typedef struct _Block {
    char *Name;           // Block name
    Key_Arr KeyArray;     // Array of keys in this block
    uint64_t line_number; // Line number in file
} Block, *PBlock;
```

**Remarks:**
- Blocks group related keys together

**Example:**

### Block_Arr

**Definition:**
```c
typedef struct _Block_Arr {
    Block *Blocks;       // Array of blocks
    uint64_t BlockCount; // Number of blocks
} Block_Arr, *PBlock_Arr;
```

### ATRC_FD

**Definition:**
```c
typedef struct _ATRCFiledata {
    Variable_Arr VariableArray; // All variables in the file
    Block_Arr BlockArray;       // All blocks in the file
    char *Filename;             // Path to the ATRC file
    bool AutoSave;              // Whether to auto-save changes
    bool Writecheck;            // Whether to verify writes
} ATRC_FD, *PATRC_FD;
```

**Remarks:**
- Main structure representing an ATRC file
- AutoSave automatically writes changes to disk
- Writecheck does the following:
    - Creates a new variable or key if one doesn't exist while modifying
    - Creates the block of a key if one doesn't exist while creating one

---

## Constructor/Destructor Functions

### Create_Empty_ATRC_FD

**Definition:**
```c
ATRC_API PATRC_FD Create_Empty_ATRC_FD();
```

**Remarks:**
- Creates and initializes an empty ATRC file descriptor
- Returns NULL on failure
- To read data into the struct, use Read

**Example:**
```c
PATRC_FD fd = Create_Empty_ATRC_FD();
if (fd == NULL) {
    printf("Failed to create ATRC_FD\n");
    return 1;
}
if(!Read(fd, "config.atrc", ATRC_READ_ONLY)) {
    printf("Failed to read ATRC_FD\n");
    Destroy_ATRC_FD(fd);
    return 0;
}
// Use fd...
Destroy_ATRC_FD(fd);
fd = NULL;
```

### Create_ATRC_FD

**Definition:**
```c
ATRC_API PATRC_FD Create_ATRC_FD(const char *filename, ReadMode readMode);
```

**Remarks:**
- Creates and initializes an ATRC file descriptor
- Returns NULL on failure
- Must be paired with `Destroy_ATRC_FD` to prevent memory leaks

**Example:**
```c
PATRC_FD fd = Create_ATRC_FD("config.atrc", ATRC_READ_ONLY);
if (fd == NULL) {
    printf("Failed to create ATRC_FD\n");
    return 1;
}
// Use fd...
Destroy_ATRC_FD(fd);
fd = NULL;
```

### Destroy_ATRC_FD

**Definition:**
```c
ATRC_API void Destroy_ATRC_FD(PATRC_FD fd);
```

**Remarks:**
- Frees all memory associated with an ATRC file descriptor
- Should be called for every successful `Create_ATRC_FD` call

**Example:**
```c
PATRC_FD fd = Create_ATRC_FD("config.atrc", ATRC_READ_ONLY);
if (fd == NULL) {
    printf("Failed to create ATRC_FD\n");
    return 1;
}
// Use fd...
Destroy_ATRC_FD(fd);
fd = NULL;
```

---

## File Operations

### Read

**Definition:**
```c
ATRC_API bool Read(PATRC_FD fd, const char* path, ReadMode readMode);
```

**Remarks:**
- Reads an ATRC file into the file descriptor
- Returns true on success, false on failure
- Can be called multiple times to reload the file

**Example:**
```c
if (!Read(fd, "config.atrc", ATRC_READ_ONLY)) {
    printf("Failed to read ATRC file\n");
    Destroy_ATRC_FD(fd);
    return 1;
}
```

---

## Data Access Functions

### ReadVariable

**Definition:**
```c
ATRC_API char* ReadVariable(PATRC_FD fd, const char* varname);
```

**Remarks:**
- Retrieves the value of a variable by name
- Returns NULL if variable doesn't exist
- Returned value must be freed with ATRC memory freeing functions

**Example:**
```c
char* username = ReadVariable(fd, "username");
if (username != NULL) {
    printf("Username: %s\n", username);
}
ATRC_FREE_MEMORY(username);
```

### ReadKey

**Definition:**
```c
ATRC_API char* ReadKey(PATRC_FD fd, const char* block, const char* key);
```

**Remarks:**
- Retrieves the value of a key within a specific block
- Returns NULL if block or key doesn't exist
- Returned value must be freed with ATRC memory freeing functions

**Example:**
```c
char* host = ReadKey(fd, "ServerConfig", "host");
if (host != NULL) {
    printf("Server host: %s\n", host);
}
ATRC_FREE_MEMORY(host);
```

---

## Existence Check Functions

### DoesExistBlock

**Definition:**
```c
ATRC_API bool DoesExistBlock(PATRC_FD fd, const char* block);
```

**Remarks:**
- Checks if a block exists in the file
- Useful for conditional logic based on configuration structure

**Example:**
```c
if (DoesExistBlock(fd, "DatabaseConfig")) {
    printf("Database configuration found\n");
}
```

### DoesExistVariable

**Definition:**
```c
ATRC_API bool DoesExistVariable(PATRC_FD fd, const char* varname);
```

**Remarks:**
- Checks if a variable exists in the file
- Even though a private variable by `varname` exists, false will be returned

**Example:**
```c
if (DoesExistVariable(fd, "debug_mode")) {
    const char* debug = ReadVariable(fd, "debug_mode");
    // Process debug setting
}
```

### DoesExistKey

**Definition:**
```c
ATRC_API bool DoesExistKey(PATRC_FD fd, const char* block, const char* key);
```

**Remarks:**
- Checks if a key exists within a specific block
- Both block and key must exist for this to return true

**Example:**
```c
if (DoesExistKey(fd, "ServerConfig", "port")) {
    const char* port = ReadKey(fd, "ServerConfig", "port");
    int port_num = atoi(port);
}
```

### IsPublic

**Definition:**
```c
ATRC_API bool IsPublic(PATRC_FD fd, const char* varname);
```

**Remarks:**
- Checks if a variable is marked as public
- Public variables can be accessed across different scopes

**Example:**
```c
if (IsPublic(fd, "global_setting")) {
    printf("Global setting is publicly accessible\n");
}
```

---

## Variable Injection

### InsertVar_S

**Definition:**
```c
ATRC_API char* InsertVar_S(const char* line, const char** args);
```

**Remarks:**
- Performs variable injection on a string template
- Uses `%*%` for sequential injection, `%*N%` for indexed injection
- Returns newly allocated string that must be freed with `ATRC_FREE_MEMORY`

**Example:**
```c
const char* template = "Hello %*%, welcome to %*%!";
const char* args[] = {"John", "ATRC", NULL};
const char* result = InsertVar_S(template, args);
printf("%s\n", result); // "Hello John, welcome to ATRC!"
ATRC_FREE_MEMORY(result);
result = NULL;
```

---

## Block Management

### AddBlock

**Definition:**
```c
ATRC_API bool AddBlock(PATRC_FD fd, const char* blockname);
```

**Remarks:**
- Adds a new block to the ATRC file
- Returns false if block already exists or on memory allocation failure
- Block names must be unique
- If AutoSave is turned on, the block will be added after fileheader, variables and comments

**Example:**
```c
if (AddBlock(fd, "NewSection")) {
    printf("Block 'NewSection' added successfully\n");
}
```

### RemoveBlock

**Definition:**
```c
ATRC_API bool RemoveBlock(PATRC_FD fd, const char* blockname);
```

**Remarks:**
- Removes a block and all its keys from the file
- Returns false if block doesn't exist or an error with memory allocation happens
- This operation cannot be undone
- If AutoSave is on, everything below the block will be removed until end of file or another block is found

**Example:**
```c
if (RemoveBlock(fd, "OldSection")) {
    printf("Block 'OldSection' removed successfully\n");
}
```

---

## Variable Management

### AddVariable

**Definition:**
```c
ATRC_API bool AddVariable(PATRC_FD fd, const char* varname, const char* value);
```

**Remarks:**
- Adds a new variable to the ATRC file
- Returns false if variable already exists or memory allocation failures happen
- Variables are global within the file
- If AutoSave is on, the variable will be added after the fileheader and comments

**Example:**
```c
if (AddVariable(fd, "app_version", "2.3.2")) {
    printf("Variable 'app_version' added\n");
}
```

### RemoveVariable

**Definition:**
```c
ATRC_API bool RemoveVariable(PATRC_FD fd, const char* varname);
```

**Remarks:**
- Removes a variable from the file
- Returns false if variable doesn't exist

**Example:**
```c
if (RemoveVariable(fd, "temp_setting")) {
    printf("Temporary setting removed\n");
}
```

### ModifyVariable

**Definition:**
```c
ATRC_API bool ModifyVariable(PATRC_FD fd, const char* varname, const char* value);
```

**Remarks:**
- Changes the value of an existing variable
- Returns false if variable doesn't exist
- Use `AddVariable` for new variables
- If WriteCheck is on, the new variable will be created with the given values

**Example:**
```c
if (ModifyVariable(fd, "app_version", "2.4.0")) {
    printf("App version updated\n");
}
```

---

## Key Management

### AddKey

**Definition:**
```c
ATRC_API bool AddKey(PATRC_FD fd, const char* block, const char* key, const char* value);
```

**Remarks:**
- Adds a new key-value pair to a specific block
- Block must exist before adding keys
- Returns false if key already exists in the block

**Example:**
```c
if (AddKey(fd, "ServerConfig", "timeout", "30")) {
    printf("Timeout setting added\n");
}
```

### RemoveKey

**Definition:**
```c
ATRC_API bool RemoveKey(PATRC_FD fd, const char* block, const char* key);
```

**Remarks:**
- Removes a key from a specific block
- Returns false if block or key doesn't exist

**Example:**
```c
if (RemoveKey(fd, "ServerConfig", "old_setting")) {
    printf("Old setting removed\n");
}
```

### ModifyKey

**Definition:**
```c
ATRC_API bool ModifyKey(PATRC_FD fd, const char* block, const char* key, const char* value);
```

**Remarks:**
- Changes the value of an existing key in a block
- Returns false if block or key doesn't exist

**Example:**
```c
if (ModifyKey(fd, "ServerConfig", "port", "8080")) {
    printf("Server port updated\n");
}
```

---

## Comment Functions

### WriteCommentToBottom

**Definition:**
```c
ATRC_API bool WriteCommentToBottom(PATRC_FD fd, const char* comment);
```

**Remarks:**
- Adds a comment line to the end of the file
- AutoSave is required to be on

**Example:**
```c
WriteCommentToBottom(fd, "Configuration updated on 2025-07-29");
```

### WriteCommentToTop

**Definition:**
```c
ATRC_API bool WriteCommentToTop(PATRC_FD fd, const char* comment);
```

**Remarks:**
- Adds a comment line to the beginning of the file, below the fileheader
- AutoSave is required to be one

**Example:**
```c
WriteCommentToTop(fd, "LOG UPDATE: Changed key value");
```

---

## Enum Functions

### GetEnumValue

**Definition:**
```c
ATRC_API uint64_t GetEnumValue(PATRC_FD fd, const char* block, const char* key);
```

**Remarks:**
- Returns the enumeration value for a key within a block
- First key in a block has enum value 0, second has 1, etc.
- Useful for switch statements or array indexing

**Example:**
```c
uint64_t status_enum = GetEnumValue(fd, "StatusConfig", "current_status");
switch (status_enum) {
    case 0: printf("Status: Active\n"); break;
    case 1: printf("Status: Inactive\n"); break;
    case 2: printf("Status: Pending\n"); break;
}
```

---

## Memory Management

### ATRC_FREE_MEMORY

**Definition:**
```c
ATRC_API void ATRC_FREE_MEMORY(void *ptr);
```

**Remarks:**
- Safely frees memory allocated by ATRC functions

**Example:**
```c
const char* result = InsertVar_S(template, args);
// Use result...
ATRC_FREE_MEMORY(result);
result = null;
```

### Copy_ATRC_FD

**Definition:**
```c
ATRC_API PATRC_FD Copy_ATRC_FD(PATRC_FD fd);
```

**Remarks:**
- Copies ATRC_FD's memory into a new structure

**Example:**
```c
PATRC_FD copy = Copy_ATRC_FD(old_fd);
Destroy_ATRC_FD(fd);
Destroy_ATRC_FD(copy);
```

---

## Macro usage

Macros for C API are defined as follows:

__<FUNCTION_NAME_IN_UPPERCASE>

The function macros allow user to make shorter function calls.
Before calling these macros, `GLB_ATRC_FD` needs to be defined to a valid ATRC_FD pointer

```c
#include <ATRC.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    PATRC_FD fd = Create_ATRC_FD("config.atrc", ATRC_CREATE_READ);
    if(fd == NULL) {
        return 1;
    }
    #define GLB_ATRC_FD fd
    char *res = __READ_VARIABLE("VarName");
    /*
    Macro expands into:
    char *res = ReadVariable(GLB_ATRC_FD, "varName");
    */

   ATRC_FREE_MEMORY(res);
}
```


---

## Complete Example

```c
#include "ATRC.h"
#include <stdio.h>

int main() {
    // Create ATRC file descriptor
    PATRC_FD fd = Create_ATRC_FD("config.atrc", ATRC_CREATE_READ);
    if (fd == NULL) {
        printf("Failed to create ATRC_FD\n");
        return 1;
    }

    // Read the file
    if (!Read(fd, "config.atrc", ATRC_READ_ONLY)) {
        printf("Failed to read file\n");
        Destroy_ATRC_FD(fd);
        return 1;
    }

    // Add a variable
    AddVariable(fd, "app_name", "MyApplication");

    // Add a block and keys
    AddBlock(fd, "ServerConfig");
    AddKey(fd, "ServerConfig", "host", "localhost");
    AddKey(fd, "ServerConfig", "port", "8080");

    // Read values
    char* app_name = ReadVariable(fd, "app_name");
    char* host = ReadKey(fd, "ServerConfig", "host");
    
    printf("Application: %s\n", app_name ? app_name : "Unknown");
    printf("Host: %s\n", host ? host : "Unknown");

    // Variable injection example
    const char* template = "Connecting to %*% on port %*%";
    const char* args[] = {host, ReadKey(fd, "ServerConfig", "port"), NULL};
    const char* message = InsertVar_S(template, args);
    printf("%s\n", message);
    ATRC_FREE_MEMORY(message);
    ATRC_FREE_MEMORY(app_name);
    ATRC_FREE_MEMORY(host);
    message = NULL;

    // Clean up
    Destroy_ATRC_FD(fd);
    return 0;
}
```
