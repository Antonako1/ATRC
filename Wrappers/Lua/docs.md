# ATRC Lua API documentation

fd = filedata

## fd OpenATRC(string path)
Reads the given atrc file and saves its data into fd

## CloseATRC(fd)
Closes ATRC file, freeing its allocated memory

## bool ReadATRC(fd, string path)
Reads a new ATRC file, returning TRUE on success, FALSE otherwise

## void SetAutosave(fd, bool newStatus)
Sets autosave status. AutoSave automatically writes changes to disk
## bool GetAutosave(fd) 
Gets autosave status

## string GetFilename(fd)
Gets filename from filedata

## num GetVariableCount(fd)
Returns total amount of variables

## void SetWriteCheck(fd, bool newStatus)
Sets writecheck status. Writecheck does the following:
 - Creates a new variable or key if one doesn't exist while modifying
 - Creates the block of a key if one doesn't exist while creating one

## bool GetWriteCheck(fd)
Gets writecheck status

## string ReadVariable(fd, string varname)
Reads variables value

## string ReadKey(fd, string block, string key)
Reads key's value from the block

## bool DoesExistBlock(fd, string block)
Checks whether block exists. True if exists

## bool DoesExistVariable(fd, string varname)
Checks whether variable exists. True if exists

## bool DoesExistKey(fd, string block, string key)
Checks whether a key exists inside a block. True if exists

## bool IsPublic(fd, string varname)
Checks whether a variable is public. True if public

## string InsertVars(string line, string[] args)
Inserts args into line
```lua
local args = {"Argument 1", "Argument 2!"}
local line = m.ReadKey(fd, "Block", "Inserts")
print(line) -- Output: "%*%, %*%"
res = m.InsertVars(line, args)
print(res) -- Output: "Argument 1, Argument 2!"
```

## bool AddBlock(fd, string block)
Adds a new block.
If autosave is on, adds a new block to the top of the file, below first comments and variable declarations

## bool RemoveBlock(fd, string block)
Removes block and its keys
If autosave is on, everything below the block will be deleted until the end of file or another block is found

## bool AddKey(fd, string block, string key, string value)
Adds a new key inside a block
If autosave is on, the key is added just below the block

## bool RemoveKey(fd, string block, string key)
Removes a key from a block

## bool ModifyKey(fd, string block, string key, string value)
Modifies key's value

## bool AddVariable(fd, string varname, string value)
Creates a new variable.
If autosave is on, it will be added below first comments

## bool RemoveVariable(fd, string varname)
Removes a variable

## bool ModifyVariable(fd, string varname, string value)
Modifies variable's value

## bool WriteCommentToBottom(fd, string comment)
Writes a comment to the bottom of the file. Autosave is required to be on

## bool WriteCommentToTop(fd, string comment)
Writes a comment to the top of the file, just above fileheader. Autosave is required to be on

## num GetEnumValue(fd, string block, string key)
Gets emum value of a key inside a block

## fd CopyATRC(fd)
Copies filedata into another. Both copies will need to be freed with CloseATRC()

## bool StrToBool(string value)
Turns string into boolean value.
Values accepted:
 - On, True, 1
 - Off, False, 0
Values are case-insensitive

## num StrToNum(string value)
Turns a value into number

## num StrToDouble(string value)
Turns a value containing floating point numbers into num

## double ParseMathExp(string value)
Parser math expressions (1+2*3/4)
