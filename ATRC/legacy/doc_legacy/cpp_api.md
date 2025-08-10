# ATRC C++ API Documentation

## Table of Contents



- [Enums](#enums)
  - [ReadMode](#readmode)
- [Data Structures](#data-structures)
  - [Variable](#variable)
  - [Key](#key)
  - [Block](#block)
- [ATRC_FD](#atrc_fd)
  - [ATRC_FD (Default Constructor)](#atrc_fd-default-constructor)
  - [ATRC_FD (String Path Constructor)](#atrc_fd-string-path-constructor)
  - [ATRC_FD (Char Path Constructor)](#atrc_fd-char-path-constructor)
  - [ATRC_FD (C Struct Constructor)](#atrc_fd-c-struct-constructor)
  - [~ATRC_FD](#atrc_fd-1)
  - [ReadAgain](#readagain)
  - [Read](#read)
  - [ReadVariable](#readvariable)
  - [ReadKey](#readkey)
  - [DoesExistBlock](#doesexistblock)
  - [DoesExistVariable](#doesexistvariable)
  - [DoesExistKey](#doesexistkey)
  - [IsPublic](#ispublic)
  - [InsertVar](#insertvar)
  - [InsertVar_S](#insertvar_s)
  - [AddBlock](#addblock)
  - [RemoveBlock](#removeblock)
  - [AddVariable](#addvariable)
  - [RemoveVariable](#removevariable)
  - [ModifyVariable](#modifyvariable)
  - [AddKey](#addkey)
  - [RemoveKey](#removekey)
  - [ModifyKey](#modifykey)
  - [WriteCommentToTop](#writecommenttotop)
  - [WriteCommentToBottom](#writecommenttobottom)
  - [GetEnumValue](#getenumvalue)
  - [CheckStatus](#checkstatus)
  - [GetVariables](#getvariables)
  - [GetBlocks](#getblocks)
  - [GetFilename](#getfilename)
  - [GetAutoSave](#getautosave)
  - [GetWriteCheck](#getwritecheck)
  - [SetAutoSave](#setautosave)
  - [SetWriteCheck](#setwritecheck)
  - [ToCStruct](#tocstruct)
  - [operator[]](#operator)
- [PROXY_ATRC_FD](#proxy_atrc_fd)
  - [operator[] (Proxy)](#operator-proxy)
  - [operator std::string](#operator-stdstring)
  - [operator const char*](#operator-const-char)
  - [operator=](#operator-1)
  - [operator>>](#operator-2)
  - [operator<<](#operator-3)
- [Memory Management](#memory-management)
  - [ATRC_FREE_MEMORY](#atrc_free_memory)

---

## Enums

### ReadMode

ReadMode is used by the [Constructors](#constructors) and [ReadAgain](#ReadAgain)

```cpp
typedef enum _ReadMode {
    ATRC_READ_ONLY,
    ATRC_CREATE_READ,
    ATRC_FORCE_READ,
} ReadMode;
```

---

## Data Structures

### Variable

Variable structure

```cpp
typedef struct ATRC_API _Variable {
    std::string Name;
    std::string Value;
    bool IsPublic = true;
    uint64_t line_number;
} Variable, * PVariable;
```

#### Name
Name of the variable
#### Value
Variable's value
#### IsPublic
Is the variable public. See more about private and public variables in `syntax.md`-`Variables`
#### line_number
Line number of the variable

### Key

Key's structure definition

```cpp
typedef struct ATRC_API _Key {
    std::string Name;
    std::string Value;
    uint64_t line_number;
    uint64_t enum_value;
} Key, * PKey;
```

#### Name
Name of the key
#### Value
Key's value
#### line_number
Line number of the key
#### enum_value
Enum value of the key. For more information, see `advanced_features.md`-`Enums`

### Block

Block structure definition

```cpp
typedef struct ATRC_API _Block {
    std::string Name;
    std::vector<Key> Keys;
    uint64_t line_number;
} Block, * PBlock;
```

#### Name
Name of the block
#### Keys
Array of keys
#### line_number
Line number of the block

---

## ATRC_FD

Main class for handling ATRC configuration files.

```cpp
class ATRC_API ATRC_FD {
public:
    // ... (see individual methods below)
};
typedef ATRC_FD* PATRC_FD;
```

### ATRC_FD (Default Constructor)

Creates empty ATRC_FD class

```cpp
ATRC_FD();
```

### ATRC_FD (String Path Constructor)

Creates ATRC_FD class and reads the given configuration file.

```cpp
ATRC_FD(std::string& path, ReadMode mode = ATRC_READ_ONLY);
```

#### path
Path to the file
#### mode
ReadMode. See [ReadMode](#readmode)

### ATRC_FD (Char Path Constructor)

Creates ATRC_FD class and reads the given configuration file.

```cpp
ATRC_FD(const char *path, ReadMode mode = ATRC_READ_ONLY);
```

#### path
Path to the file
#### mode
ReadMode. See [ReadMode](#readmode)

### ATRC_FD (C Struct Constructor)

Creates a new ATRC_FD class from a C_ATRC_FD file structure.
See `c_api.md`-`C_ATRC_FD` for more information.
This does NOT free the C_ATRC_FD structure from memory.

```cpp
ATRC_FD(PATRC_FD filedata);
```

### ~ATRC_FD

Default deconstructor

```cpp
~ATRC_FD();
```

### ReadAgain

Reads the file again. 
Use only after calling string path, char path or c-struct construtors as no filename is saved inside the class

```cpp
bool ReadAgain(ReadMode mode = ATRC_READ_ONLY);
```

#### mode
ReadMode. See [ReadMode](#readmode)
#### Returns
TRUE on success, FALSE otherwise

### Read

Reads an ATRC file from the given path

```cpp
bool Read(std::string& path, ReadMode mode = ATRC_READ_ONLY);
```

#### path
Path to the file
#### mode
ReadMode. See [ReadMode](#readmode)
#### Returns
TRUE on success, FALSE otherwise

### ReadVariable

Reads variable's value

```cpp
std::string ReadVariable(const std::string& varname);
```

#### varname
Variable to read
#### Returns
Variable's value

#### Remarks
See [operator[] (Proxy)](#operator-proxy) for details on how to read variables easier!

### ReadKey

Reads the given key from the given block

```cpp
std::string ReadKey(const std::string& block, const std::string& key);
```

#### block
Block's name
#### key
Key to read
#### Returns
Key's value

#### Remarks
See [operator[] (Proxy)](#operator-proxy) for details on how to read keys easier!

### DoesExistBlock

Checks if the given block exists

```cpp
bool DoesExistBlock(const std::string& block);
```

#### block
Block to check
#### Returns
TRUE if block exists, FALSE otherwise

### DoesExistVariable
Checks if the given variable exists

```cpp
bool DoesExistVariable(const std::string& varname);
```
#### block
Variable to varname
#### Returns
TRUE if block exists, FALSE otherwise

### DoesExistKey
Checks if the given key exists

```cpp
bool DoesExistKey(const std::string& block, const std::string& key);
```
#### block
Block's name
#### key
Key to check
#### Returns
TRUE if block exists, FALSE otherwise

### IsPublic

Checks whether a variable is public or private.
See more about private and public variables in `syntax.md`-`Variables`

```cpp
bool IsPublic(const std::string& varname);
```

#### varname
Variable to check
#### Returns
TRUE if variable is public, FALSE otherwise

### InsertVar

Inserts values into inject markings. 
See `syntax.md`-`Insert/Inject Markings` for more information
For easier usage, see the macro `ATRC_INJECT` in `macros_constants.md`


```cpp
void InsertVar(std::string& line, std::vector<std::string>& args);
```

#### line
Line containing inject markings. The result will be saved into this variable
#### args
Array of injects

#### Example
```cpp
std::string line = "String containing %*% markings";
std::vector<std::string> injects = {"inject"}
fd.InsertVar(line, injects);
std::cout << line << std::endl; // -> String containing inject markings
```

### InsertVar_S

Inserts values into inject markings. 
See `syntax.md`-`Insert/Inject Markings` for more information
For easier usage, see the macro `ATRC_INJECT` in `macros_constants.md`

```cpp
std::string InsertVar_S(const std::string& line, std::vector<std::string>& args);
```

#### line
Line containing inject markings.
#### args
Array of injects
#### Returns 
Injected line

#### Example
```cpp
std::string line = "String containing %*% markings";
std::vector<std::string> injects = {"inject"}
std::string res = fd.InsertVar_S(res, injects);
std::cout << line << std::endl; // -> String containing inject markings
```

### AddBlock

Creates a new block. 
If AutoSave is turned on, the block will be added to the top of the file, after fileheader, comments and variable definitions.

```cpp
bool AddBlock(const std::string& blockname);
```

#### blockname
Block to add
#### Returns
TRUE on success, FALSE otherwise

### RemoveBlock

Removes a block. 
If autosave is turned on, everything below the blocks definition is deleted until a second block definition is found. This includes comments and variables

```cpp
bool RemoveBlock(const std::string& blockname);
```

#### blockname
Block to remove
#### Returns
TRUE on success, FALSE otherwise

### AddVariable

Creates a new variable.
If autosave is on, the new variable will be added after the fileheader and comments.

```cpp
bool AddVariable(const std::string& varname, const std::string& value);
```

#### varname
Variable's name
#### value
Variable's value
#### Returns
TRUE on success, FALSE otherwise

### RemoveVariable

Deletes a variable

```cpp
bool RemoveVariable(const std::string& varname);
```

#### varname
Variable to remove
#### Returns
TRUE on success, FALSE otherwise

### ModifyVariable

Modifies a variable's value with a given new one

```cpp
bool ModifyVariable(const std::string& varname, const std::string& value);
```

#### varname
Variable to modify
#### value
New value
#### Returns
TRUE on success, FALSE otherwise

### AddKey

Adds a new key into a block.
If autosave is turned on, the key will be added right after the block definition.

```cpp
bool AddKey(const std::string& block, const std::string& key, const std::string& value);
```

#### block
Block's name
#### key
Key to add
#### value
Key's value
#### Returns
TRUE on success, FALSE otherwise

### RemoveKey

Remove's a key from the given block

```cpp
bool RemoveKey(const std::string& block, const std::string& key);
```

#### block
Block's name
#### key
Key to remove
#### Returns
TRUE on success, FALSE otherwise

### ModifyKey

Modifies a key from a block with the new given value

```cpp
bool ModifyKey(const std::string& block, const std::string& key, const std::string& value);
```

#### block
Block's name
#### key
Key to modify
#### value
New value
#### Returns
TRUE on success, FALSE otherwise

### WriteCommentToTop

Writes a comment to the top of the file, just below the fileheader

```cpp
bool WriteCommentToTop(const std::string& comment);
```

#### comment
Comment to write
#### Returns
TRUE on success, FALSE otherwise

#### Remarks
If the comment written is "This is a comment", the comment will look like this inside the file: "#This is a comment". 

### WriteCommentToBottom

Writes a comment on the last row of the file

```cpp
bool WriteCommentToBottom(const std::string& comment);
```

#### comment
Comment to write
#### Returns
TRUE on success, FALSE otherwise

#### Remarks
If the comment written is "This is a comment", the comment will look like this inside the file: "#This is a comment". 

### GetEnumValue

Gets the enum value of a key from a block. 
For more information, see `advanced_features.md`-`Enums`

```cpp
uint64_t GetEnumValue(const std::string& block, const std::string& key);
```
#### block
Block's name
#### key
Key's name
#### Returns
Enum value of the key. (uint64_t)-1 on errors

### CheckStatus

Check's status of ATRC_FD class, disclosing if it is safe to use

```cpp
bool CheckStatus();
```

#### Returns
TRUE, ATRC_FD is safe to use. FALSE, not safe

### GetVariables

Gets vector of variables

```cpp
std::vector<Variable> GetVariables();
```

#### Returns
An array of variables. All the private variables have been remove.
See more about private and public variables in `syntax.md`-`Variables`

### GetBlocks

Gets all blocks 

```cpp
std::vector<Block>* GetBlocks();
```

#### Returns 
Pointer to the vector of blocks

### GetFilename

Gets the filename

```cpp
std::string GetFilename();
```

#### Returns
Filename; The file that has been read

### GetAutoSave

Gets autosave status

When AutoSave is set to true, all changes to the variables, keys 
and blocks are saved into the file.
When AutoSave is false, changes will not be saved into the file.
  False by default

```cpp
bool GetAutoSave() const;
```

#### Returns
AutoSave status

### GetWriteCheck

Gets WriteCheck status

If writecheck is true and user is modifying a variable, it will create a new one if one doesn't exist while modifying it.
If writecheck is true and user is modifying a key, it will create a new block and key if one doesn't exist while modifying it. 

```cpp
bool GetWriteCheck() const;
```

#### Returns
Writecheck status

### SetAutoSave

Sets autosave

When AutoSave is set to true, all changes to the variables, keys 
and blocks are saved into the file.
When AutoSave is false, changes will not be saved into the file.
  False by default

```cpp
void SetAutoSave(bool autosave);
```

#### autosave
New autosave status

### SetWriteCheck

Sets WriteCheck status

If writecheck is true and user is modifying a variable, it will create a new one if one doesn't exist while modifying it.
If writecheck is true and user is modifying a key, it will create a new block and key if one doesn't exist while modifying it.

```cpp
void SetWriteCheck(bool writecheck);
```

#### writecheck
New WriteCheck status

### ToCStruct

Creates a new C_ATRC_FD structure from the data of the ATRC_FD class.
See `c_api.md`-`C_ATRC_FD` for more information.
Free with Destroy_ATRC_FD. See `c_api.md`-`Destroy_ATRC_FD` for more information.

```cpp
PATRC_FD ToCStruct();
```

#### Returns
Pointer to C_ATRC_FD structure

### operator[]

Used to read values with [] operators. See [PROXY_ATRC_FD](#PROXY_ATRC_FD) for more information on these operators

```cpp
PROXY_ATRC_FD operator[](const std::string& key);
PROXY_ATRC_FD operator[](const std::string& key) const;
```

---

## PROXY_ATRC_FD

Proxy class for convenient access to ATRC data using operator syntax.

This class is intended for "inner-usage" only, meaning do not fiddle or create this class, as it's only usage is to operate the operators of ATRC_FD class!

### operator[] (Proxy)

Allows the reading with [] operators. See example below.

```cpp
PROXY_ATRC_FD operator[](const std::string& subKey);
```

#### Examples
```cpp
// Read value of a variable "VariableName"
std::string VarValue = fd["VariableName"];
// Read value of "KeyName" from block "BlockName"
std::string KeyValue = fd["BlockName"]["KeyName"];

// If/When the []-operator returns a const char*, it needs to be freed,
//  for this use ATRC_FREE_MEMORY
const char *VarValue2 = fd["VariableName2"];
const char *KeyValue2 = fd["BlockName"]["KeyName2"];
ATRC_FREE_MEMORY(VarValue2);
ATRC_FREE_MEMORY(KeyValue2);
```

### operator=

Assign a value into []-operator. See example below.

```cpp
PROXY_ATRC_FD& operator=(const std::string& value);
```

#### Example
```cpp
fd["VarName"] = "New value!"
fd["BlockName"]["KeyName"] = "Another new value!";
```

### operator>>

Appends data into []-operator. See example below.

```cpp
PROXY_ATRC_FD& operator>>(const std::string& value);
PROXY_ATRC_FD& operator>>(const char* value);
```

#### Example
```cpp
fd["VarName"] >> "this is appended!".
fd["BlockName"]["KeyName"] >> "this too is appended!";
```

### operator<<

Allows program to input output from []-operator into std::cout. See example below.

```cpp
inline friend std::ostream& operator<<(std::ostream& os, const PROXY_ATRC_FD& obj);
```

#### Example
```cpp
std::cout << fd["VarName"] << std::endl;
std::cout << fd["BlockName"]["KeyName"] << std::endl;
```

---

## Memory Management

### ATRC_FREE_MEMORY

Frees memory allocated by ATRC. Never use free() or delete[] with ATRC if not told otherwise!

```c
ATRC_API void *ATRC_FREE_MEMORY(void *ptr);
```

#### ptr
Memory to free
#### Returns
Pointer to `ptr`. NULL on success, anything else otherwise
