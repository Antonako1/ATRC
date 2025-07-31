#define _CRT_SECURE_NO_DEPRECATE
#include "./include/ATRC.h"
#include "./include/filer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


uint64_t GetEnumValue(PATRC_FD fd, const char* block, const char* key) {
    for(uint64_t i = 0; i < fd->BlockArray.BlockCount; i++) {
        PBlock blk = &fd->BlockArray.Blocks[i];
        if(strcmp(blk->Name, block) == 0) {
            for(uint64_t j = 0; j < blk->KeyArray.KeyCount; j++) {
                PKey k = &blk->KeyArray.Keys[j];
                if(strcmp(k->Name, key) == 0) {
                    return k->enum_value;
                }
            }
        }
	}
	return INVALID_INDEX;
}

char* ReadVariable(PATRC_FD fd, const char* varname) {
    char* res = NULL;
    for(uint64_t i = 0; i < fd->VariableArray.VariableCount; i++) {
        PVariable var = &fd->VariableArray.Variables[i];
        if(strcmp(var->Name, varname) == 0) {
            res = __STRDUP(var->Value);
            return res;
        }
	}
    return NULL;
}

char* ReadKey(PATRC_FD fd, const char* block, const char* key) {
    char* res = NULL;
    for (uint64_t i = 0; i < fd->BlockArray.BlockCount; i++) {
        PBlock blk = &fd->BlockArray.Blocks[i];
        if (strcmp(blk->Name, block) == 0) {
            for (uint64_t j = 0; j < blk->KeyArray.KeyCount; j++) {
                PKey k = &blk->KeyArray.Keys[j];
                if (strcmp(k->Name, key) == 0) {
                    res = __STRDUP(k->Value);
                    return res;
                }
            }
        }
    }
    return res;
}

bool DoesExistBlock(PATRC_FD fd, const char* block) {
    bool res = false;
    for(uint64_t i = 0; i < fd->BlockArray.BlockCount; i++) {
        PBlock blk = &fd->BlockArray.Blocks[i];
        if(strcmp(blk->Name, block) == 0) {
            res = true;
            break;
		}
        }
	return res;
}

bool DoesExistVariable(PATRC_FD fd, const char* varname) {
    bool res = false;
    for(uint64_t i = 0; i < fd->VariableArray.VariableCount; i++) {
        PVariable var = &fd->VariableArray.Variables[i];
        if(strcmp(var->Name, varname) == 0) {
            res = true;
            if(var->IsPublic == false) {
                _ATRC_WRAP_ERRORMSG(ERR_UNAUTHORIZED_ACCESS_TO_VAR, -1, varname, fd->Filename);
                res = false;
			}
			break;
        }
    }
	return res;
}

bool DoesExistKey(PATRC_FD fd, const char* block, const char* key) {
    bool res = false;
    for(uint64_t i = 0; i < fd->BlockArray.BlockCount; i++) {
        PBlock blk = &fd->BlockArray.Blocks[i];
        if(strcmp(blk->Name, block) == 0) {
            for(uint64_t j = 0; j < blk->KeyArray.KeyCount; j++) {
                PKey k = &blk->KeyArray.Keys[j];
                if(strcmp(k->Name, key) == 0) {
                    res = true;
                    break;
                }
            }
            if(res) break;
        }
	}
	return res;
}

bool IsPublic(PATRC_FD fd, const char* varname) {
    bool res = false;
    for(uint64_t i = 0; i < fd->VariableArray.VariableCount; i++) {
        PVariable var = &fd->VariableArray.Variables[i];
        if(strcmp(var->Name, varname) == 0) {
            res = var->IsPublic;
			break;
        }
    }
	return res;
}
/*
NOTE: If added someday, check _ATRC_WRAP_INSERTVAR imementation for correct usage.
void InsertVar(char* line, const char** args) {
    _ATRC_WRAP_INSERTVAR(line, args);
}
*/

char* InsertVar_S(const char* line, const char** args) {
    return _ATRC_WRAP_INSERTVAR_S(line, args);
}

bool AddBlock(PATRC_FD fd, const char* blockname) {
    if (DoesExistBlock(fd, blockname)) {
		_ATRC_WRAP_ERRORMSG(ERR_BLOCK_EXISTS, -1, blockname, fd->Filename);
        return false;
    }

	size_t new_count = fd->BlockArray.BlockCount + 1;
    if(new_count > SIZE_MAX / sizeof(Block)) {
        _ATRC_WRAP_ERRORMSG(ERR_MEMORY_ALLOCATION_FAILED, __LINE__, "Block array size exceeds maximum", fd->Filename);
        return false;
	}

	PBlock temp = (PBlock)realloc(fd->BlockArray.Blocks, new_count * sizeof(Block));
    if (temp == NULL) {
        _ATRC_WRAP_ERRORMSG(ERR_MEMORY_ALLOCATION_FAILED, __LINE__, "Block array allocating error", fd->Filename);
		return false;
    }
	fd->BlockArray.Blocks = temp;

    PBlock new_block = &fd->BlockArray.Blocks[fd->BlockArray.BlockCount];
    new_block->Name = __STRDUP(blockname);
    if (new_block->Name == NULL) {
        _ATRC_WRAP_ERRORMSG(ERR_MEMORY_ALLOCATION_FAILED, __LINE__, "Block name allocating error", fd->Filename);
		
        // Realloc to shrink the array
        PBlock shrinked = (PBlock)realloc(fd->BlockArray.Blocks, fd->BlockArray.BlockCount * sizeof(Block));
        if (shrinked != NULL || fd->BlockArray.BlockCount == 0) {
            fd->BlockArray.Blocks = shrinked;
        }
        else {
            _ATRC_WRAP_ERRORMSG(ERR_MEMORY_ALLOCATION_FAILED, __LINE__, "Block array rollback realloc failed", fd->Filename);
        }

        return false;
    }
    new_block->KeyArray.Keys = NULL;
    new_block->KeyArray.KeyCount = 0;
	new_block->line_number = 0; // Set to 0 or appropriate line number

    fd->BlockArray.BlockCount = new_count;

    if (fd->AutoSave) {
        _ATRC_WRAP__W_SAVE(fd, ADD_BLOCK, -1, blockname, NULL, NULL);
    }
	return true;
}

bool RemoveBlock(PATRC_FD fd, const char* blockname) {
    if(!DoesExistBlock(fd, blockname)) {
        _ATRC_WRAP_ERRORMSG(ERR_BLOCK_NOT_FOUND, -1, blockname, fd->Filename);
        return false;
	}

	size_t index = SIZE_MAX;
    for (size_t i = 0; i < fd->BlockArray.BlockCount; i++) {
        if (strcmp(fd->BlockArray.Blocks[i].Name, blockname) == 0) {
            index = i;
            break;
        }
	}

    if (index == SIZE_MAX) {
        _ATRC_WRAP_ERRORMSG(ERR_BLOCK_NOT_FOUND, -1, blockname, fd->Filename);
        return false;
	}

	// Free the block name
	__ATRC_FREE_MEMORY_EX(fd->BlockArray.Blocks[index].Name);

	// Free the keys in the block
	for (uint64_t j = 0; j < fd->BlockArray.Blocks[index].KeyArray.KeyCount; j++) {
		PKey key = &fd->BlockArray.Blocks[index].KeyArray.Keys[j];
		__ATRC_FREE_MEMORY_EX(key->Name);
		__ATRC_FREE_MEMORY_EX(key->Value);
	}

	// Free the keys array
	__ATRC_FREE_MEMORY_EX(fd->BlockArray.Blocks[index].KeyArray.Keys);

	// Shift the remaining blocks
    for (size_t i = index; i < fd->BlockArray.BlockCount - 1; i++) {
		fd->BlockArray.Blocks[i] = fd->BlockArray.Blocks[i + 1];
	}

	size_t new_count = fd->BlockArray.BlockCount - 1;
	PBlock temp = (PBlock)realloc(fd->BlockArray.Blocks, new_count * sizeof(Block));
    if (temp == NULL && new_count > 0) {
        _ATRC_WRAP_ERRORMSG(ERR_MEMORY_ALLOCATION_FAILED, __LINE__, "Block array realloc error", fd->Filename);
        return false;
	}
	fd->BlockArray.Blocks = temp;
	fd->BlockArray.BlockCount = new_count;

    if (fd->AutoSave) {
		_ATRC_WRAP__W_SAVE(fd, REMOVE_BLOCK, -1, blockname, NULL, NULL);
    }
    return true;
}

bool AddVariable(PATRC_FD fd, const char* varname, const char* value) {
    if(DoesExistVariable(fd, varname)) {
        _ATRC_WRAP_ERRORMSG(ERR_VAR_EXISTS, -1, varname, fd->Filename);
        return false;
	}

    size_t new_count = fd->VariableArray.VariableCount + 1;
    if(new_count > SIZE_MAX / sizeof(Variable)) {
        _ATRC_WRAP_ERRORMSG(ERR_MEMORY_ALLOCATION_FAILED, __LINE__, "Variable array size exceeds maximum", fd->Filename);
        return false;
	}

    PVariable temp = (PVariable)realloc(fd->VariableArray.Variables, new_count * sizeof(Variable));
    if(temp == NULL) {
        _ATRC_WRAP_ERRORMSG(ERR_MEMORY_ALLOCATION_FAILED, __LINE__, "Variable array allocating error", fd->Filename);
        return false;
	}
    fd->VariableArray.Variables = temp;
    PVariable new_var = &fd->VariableArray.Variables[fd->VariableArray.VariableCount];
    new_var->Name = __STRDUP(varname);
    if(new_var->Name == NULL) {
        _ATRC_WRAP_ERRORMSG(ERR_MEMORY_ALLOCATION_FAILED, __LINE__, "Variable name allocating error", fd->Filename);
        // Realloc to shrink the array
        PVariable shrinked = (PVariable)realloc(fd->VariableArray.Variables, fd->VariableArray.VariableCount * sizeof(Variable));
        if(shrinked != NULL || fd->VariableArray.VariableCount == 0) {
            fd->VariableArray.Variables = shrinked;
        } else {
            _ATRC_WRAP_ERRORMSG(ERR_MEMORY_ALLOCATION_FAILED, __LINE__, "Variable array rollback realloc failed", fd->Filename);
        }
        return false;
    }
    new_var->Value = __STRDUP(value);
    if(new_var->Value == NULL) {
        _ATRC_WRAP_ERRORMSG(ERR_MEMORY_ALLOCATION_FAILED, __LINE__, "Variable value allocating error", fd->Filename);
        __ATRC_FREE_MEMORY_EX(new_var->Name);
        return false;
    }
    new_var->IsPublic = true; // Default to public
    new_var->line_number = 0; // Set to 0 or appropriate line number
    fd->VariableArray.VariableCount = new_count;
    if(fd->AutoSave) {
        _ATRC_WRAP__W_SAVE(fd, ADD_VAR, -1, varname, value, NULL);
	}
	return true;
}

bool RemoveVariable(PATRC_FD fd, const char* varname) {
    if (!DoesExistVariable(fd, varname)) {
        _ATRC_WRAP_ERRORMSG(ERR_VAR_NOT_FOUND, -1, varname, fd->Filename);
        return false;
    }
	size_t index = SIZE_MAX;
    for (size_t i = 0; i < fd->VariableArray.VariableCount; i++) {
        if (strcmp(fd->VariableArray.Variables[i].Name, varname) == 0) {
            index = i;
            break;
        }
    }
    if (index == SIZE_MAX) {
        _ATRC_WRAP_ERRORMSG(ERR_VAR_NOT_FOUND, -1, varname, fd->Filename);
        return false;
	}
	PVariable var = &fd->VariableArray.Variables[index];
    __ATRC_FREE_MEMORY_EX(var->Name);
	__ATRC_FREE_MEMORY_EX(var->Value);
    // Shift the remaining variables
    for (size_t i = index; i < fd->VariableArray.VariableCount - 1; i++) {
        fd->VariableArray.Variables[i] = fd->VariableArray.Variables[i + 1];
	}
    size_t new_count = fd->VariableArray.VariableCount - 1;
    PVariable temp = (PVariable)realloc(fd->VariableArray.Variables, new_count * sizeof(Variable));
    if (temp == NULL && new_count > 0) {
        _ATRC_WRAP_ERRORMSG(ERR_MEMORY_ALLOCATION_FAILED, __LINE__, "Variable array realloc error", fd->Filename);
        return false;
	}
    fd->VariableArray.Variables = temp;
    fd->VariableArray.VariableCount = new_count;
    if (fd->AutoSave) {
        _ATRC_WRAP__W_SAVE(fd, REMOVE_VAR, index, varname, NULL, NULL);
    }
	return true;
}

bool ModifyVariable(PATRC_FD fd, const char* varname, const char* value) {
    if(!DoesExistVariable(fd, varname)) {
        if(!fd->Writecheck) {
            _ATRC_WRAP_ERRORMSG(ERR_VAR_NOT_FOUND, -1, varname, fd->Filename);
			return false;
            }
        if(!AddVariable(fd, varname, value)) {
            _ATRC_WRAP_ERRORMSG(ERR_WRITECHECK, -1, varname, fd->Filename);
			return false;
            }
    }
    size_t index = SIZE_MAX;
    for (size_t i = 0; i < fd->VariableArray.VariableCount; i++) {
        if (strcmp(fd->VariableArray.Variables[i].Name, varname) == 0) {
            index = i;
            break;
        }
    }
    if (index == SIZE_MAX) {
        _ATRC_WRAP_ERRORMSG(ERR_VAR_NOT_FOUND, -1, varname, fd->Filename);
		return false;
    }
    PVariable var = &fd->VariableArray.Variables[index];
    __ATRC_FREE_MEMORY_EX(var->Value);
    var->Value = __STRDUP(value);
    if (var->Value == NULL) {
        _ATRC_WRAP_ERRORMSG(ERR_MEMORY_ALLOCATION_FAILED, __LINE__, "Variable value allocating error", fd->Filename);
        return false;
    }
    if (fd->AutoSave) {
        _ATRC_WRAP__W_SAVE(fd, MODIFY_VAR, index, varname, value, NULL);
	}
	return true;
}

bool AddKey(PATRC_FD fd, const char* block, const char* key, const char* value) {
    if(!DoesExistBlock(fd, block)) {
        if (!fd->Writecheck) {
            _ATRC_WRAP_ERRORMSG(ERR_BLOCK_NOT_FOUND, -1, block, fd->Filename);
            return false;
        }
		if (!AddBlock(fd, block)) {
			_ATRC_WRAP_ERRORMSG(ERR_WRITECHECK, -1, block, fd->Filename);
			return false;
        }
	}

    if (DoesExistKey(fd, block, key)) {
        _ATRC_WRAP_ERRORMSG(ERR_KEY_EXISTS, -1, key, fd->Filename);
        return false;
    }

	size_t block_index = SIZE_MAX;
    for (size_t i = 0; i < fd->BlockArray.BlockCount; i++) {
        if (strcmp(fd->BlockArray.Blocks[i].Name, block) == 0) {
            block_index = i;
            break;
        }
    }
    if (block_index == SIZE_MAX) {
        _ATRC_WRAP_ERRORMSG(ERR_BLOCK_NOT_FOUND, -1, block, fd->Filename);
        return false;
    }
    PBlock blk = &fd->BlockArray.Blocks[block_index];
    size_t new_count = blk->KeyArray.KeyCount + 1;
    if(new_count > SIZE_MAX / sizeof(Key)) {
        _ATRC_WRAP_ERRORMSG(ERR_MEMORY_ALLOCATION_FAILED, __LINE__, "Key array size exceeds maximum", fd->Filename);
        return false;
    }
    PKey temp = (PKey)realloc(blk->KeyArray.Keys, new_count * sizeof(Key));
    if(temp == NULL) {
        _ATRC_WRAP_ERRORMSG(ERR_MEMORY_ALLOCATION_FAILED, __LINE__, "Key array allocating error", fd->Filename);
        return false;
    }
    blk->KeyArray.Keys = temp;
    PKey new_key = &blk->KeyArray.Keys[blk->KeyArray.KeyCount];
    new_key->Name = __STRDUP(key);
    if(new_key->Name == NULL) {
        _ATRC_WRAP_ERRORMSG(ERR_MEMORY_ALLOCATION_FAILED, __LINE__, "Key name allocating error", fd->Filename);
        // Realloc to shrink the array
        PKey shrinked = (PKey)realloc(blk->KeyArray.Keys, blk->KeyArray.KeyCount * sizeof(Key));
        if(shrinked != NULL || blk->KeyArray.KeyCount == 0) {
            blk->KeyArray.Keys = shrinked;
        } else {
            _ATRC_WRAP_ERRORMSG(ERR_MEMORY_ALLOCATION_FAILED, __LINE__, "Key array rollback realloc failed", fd->Filename);
        }
        return false;
    }
    new_key->Value = __STRDUP(value);
    if(new_key->Value == NULL) {
        _ATRC_WRAP_ERRORMSG(ERR_MEMORY_ALLOCATION_FAILED, __LINE__, "Key value allocating error", fd->Filename);
        __ATRC_FREE_MEMORY_EX(new_key->Name);

        PKey shrinked = (PKey)realloc(blk->KeyArray.Keys, blk->KeyArray.KeyCount * sizeof(Key));
        if (shrinked != NULL || blk->KeyArray.KeyCount == 0) {
            blk->KeyArray.Keys = shrinked;
        }
        else {
            _ATRC_WRAP_ERRORMSG(ERR_MEMORY_ALLOCATION_FAILED, __LINE__, "Key array rollback realloc failed", fd->Filename);
        }
        return false;
	}

    new_key->line_number = 0; // Set to 0 or appropriate line number
    new_key->enum_value = GetEnumValue(fd, block, key); // Set enum value if needed
    blk->KeyArray.KeyCount = new_count;
    if (fd->AutoSave) {
        _ATRC_WRAP__W_SAVE(fd, ADD_KEY, -1, key, value, block);
    }
	return true;
}

bool RemoveKey(PATRC_FD fd, const char* block, const char* key) {
    if(!DoesExistBlock(fd, block)) {
        _ATRC_WRAP_ERRORMSG(ERR_BLOCK_NOT_FOUND, -1, block, fd->Filename);
		return false;
    }
    if (!DoesExistKey(fd, block, key)) {
        _ATRC_WRAP_ERRORMSG(ERR_KEY_NOT_FOUND, -1, key, fd->Filename);
        return false;
    }
	size_t block_index = SIZE_MAX;
    for (size_t i = 0; i < fd->BlockArray.BlockCount; i++) {
        if (strcmp(fd->BlockArray.Blocks[i].Name, block) == 0) {
            block_index = i;
            break;
        }
    }
    if (block_index == SIZE_MAX) {
        _ATRC_WRAP_ERRORMSG(ERR_BLOCK_NOT_FOUND, -1, block, fd->Filename);
        return false;
	}
    PBlock blk = &fd->BlockArray.Blocks[block_index];
    size_t index = SIZE_MAX;
    for (size_t i = 0; i < blk->KeyArray.KeyCount; i++) {
        if (strcmp(blk->KeyArray.Keys[i].Name, key) == 0) {
            index = i;
            break;
        }
    }
    if (index == SIZE_MAX) {
        _ATRC_WRAP_ERRORMSG(ERR_KEY_NOT_FOUND, -1, key, fd->Filename);
        return false;
	}
    PKey key_to_remove = &blk->KeyArray.Keys[index];
    __ATRC_FREE_MEMORY_EX(key_to_remove->Name);
    __ATRC_FREE_MEMORY_EX(key_to_remove->Value);
    // Shift the remaining keys
    for (size_t i = index; i < blk->KeyArray.KeyCount - 1; i++) {
        blk->KeyArray.Keys[i] = blk->KeyArray.Keys[i + 1];
    }
    size_t new_count = blk->KeyArray.KeyCount - 1;
    PKey temp = (PKey)realloc(blk->KeyArray.Keys, new_count * sizeof(Key));
    if (temp == NULL && new_count > 0) {
        _ATRC_WRAP_ERRORMSG(ERR_MEMORY_ALLOCATION_FAILED, __LINE__, "Key array realloc error", fd->Filename);
        return false;
    }
    blk->KeyArray.Keys = temp;
    blk->KeyArray.KeyCount = new_count;
    if (fd->AutoSave) {
        _ATRC_WRAP__W_SAVE(fd, REMOVE_KEY, index, key, NULL, block);
	}
	return true;
}

bool ModifyKey(PATRC_FD fd, const char* block, const char* key, const char* value) {
    if (!DoesExistBlock(fd, block)) {
        if (!fd->Writecheck) {
            _ATRC_WRAP_ERRORMSG(ERR_BLOCK_NOT_FOUND, -1, block, fd->Filename);
            return false;
        }
        if(!AddBlock(fd, block)) {
            _ATRC_WRAP_ERRORMSG(ERR_WRITECHECK, -1, block, fd->Filename);
            return false;
		}
    }
    if(!DoesExistKey(fd, block, key)) {
        if(!fd->Writecheck) {
            _ATRC_WRAP_ERRORMSG(ERR_BLOCK_NOT_FOUND, -1, block, fd->Filename);
            return false;
        }
        if(!AddKey(fd, block, key, value)) {
            _ATRC_WRAP_ERRORMSG(ERR_WRITECHECK, -1, key, fd->Filename);
            return false;
        }
	}

	size_t block_index = SIZE_MAX;
    for (size_t i = 0; i < fd->BlockArray.BlockCount; i++) {
        if (strcmp(fd->BlockArray.Blocks[i].Name, block) == 0) {
            block_index = i;
            break;
        }
    }
    if (block_index == SIZE_MAX) {
        _ATRC_WRAP_ERRORMSG(ERR_BLOCK_NOT_FOUND, -1, block, fd->Filename);
        return false;
    }
    PBlock blk = &fd->BlockArray.Blocks[block_index];
    size_t index = SIZE_MAX;
    for (size_t i = 0; i < blk->KeyArray.KeyCount; i++) {
        if (strcmp(blk->KeyArray.Keys[i].Name, key) == 0) {
            index = i;
            break;
        }
    }
    if (index == SIZE_MAX) {
        _ATRC_WRAP_ERRORMSG(ERR_KEY_NOT_FOUND, -1, key, fd->Filename);
        return false;
    }
    PKey k = &blk->KeyArray.Keys[index];
    __ATRC_FREE_MEMORY_EX(k->Value);
    k->Value = __STRDUP(value);
    if (k->Value == NULL) {
        _ATRC_WRAP_ERRORMSG(ERR_MEMORY_ALLOCATION_FAILED, __LINE__, "Key value allocating error", fd->Filename);
        return false;
    }
    if (fd->AutoSave) {
        _ATRC_WRAP__W_SAVE(fd, MODIFY_KEY, index, key, value, block);
	}
	return true;
}

bool WriteCommentToBottom(PATRC_FD fd, const char* comment) {
    if (fd->AutoSave) {
		_ATRC_WRAP__W_SAVE(fd, WRITE_COMMENT_TO_BOTTOM, -1, comment, NULL, NULL);
        return true;
    }
    return false;
}

bool WriteCommentToTop(PATRC_FD fd, const char* comment) {
    if(fd->AutoSave) {
        _ATRC_WRAP__W_SAVE(fd, WRITE_COMMENT_TO_TOP, -1, comment, NULL, NULL);
        return true;
	}
    return false;
}

bool Read(PATRC_FD fd, const char* path, ReadMode mode) {
    if(fd->Filename != NULL) {
        __ATRC_FREE_MEMORY_EX(fd->Filename);
        fd->Filename = NULL;
    }
    fd->Filename = __STRDUP(path);
    return _ATRC_WRAP_READ(fd, fd->Filename, mode);
}

PATRC_FD Create_ATRC_FD(const char *filename, ReadMode mode){
    PATRC_FD fd = Create_Empty_ATRC_FD();
    if(fd == NULL) {
        return NULL;
	}
    fd->Filename = __STRDUP(filename);
    if(fd->Filename == NULL) {
        _ATRC_WRAP_ERRORMSG(ERR_MEMORY_ALLOCATION_FAILED, __LINE__, "Filename allocating error", "Create_ATRC_FD");
        Destroy_ATRC_FD(fd);
        return NULL;
	}
    if(!Read(fd, filename, mode)) {
        Destroy_ATRC_FD(fd);
        return NULL;
    }
    return fd;
}
PATRC_FD Create_Empty_ATRC_FD(){
    PATRC_FD fd = (PATRC_FD)malloc(sizeof(ATRC_FD));
    if(fd == NULL) {
        _ATRC_WRAP_ERRORMSG(ERR_MEMORY_ALLOCATION_FAILED, __LINE__, "ATRC_FD", "Create_Empty_ATRC_FD");
        return NULL;
	}
    fd->AutoSave = false;
    fd->Writecheck = false;
    fd->Filename = NULL;

    fd->BlockArray.BlockCount = 0;
    fd->BlockArray.Blocks = NULL;

    fd->VariableArray.VariableCount = 0;
    fd->VariableArray.Variables = NULL;
    return fd;
}

void Destroy_ATRC_FD_Variables(PATRC_FD fd) {
    if (fd->VariableArray.Variables == NULL) return;

    for (uint64_t i = 0; i < fd->VariableArray.VariableCount; i++) {
        PVariable var = &fd->VariableArray.Variables[i];
        __ATRC_FREE_MEMORY_EX(var->Name);
        __ATRC_FREE_MEMORY_EX(var->Value);
    }
    __ATRC_FREE_MEMORY_EX(fd->VariableArray.Variables);
	fd->VariableArray.VariableCount = 0;
}

void Destroy_ATRC_FD_Blocks_And_Keys(PATRC_FD fd) {
    if (fd->BlockArray.Blocks == NULL) return;

    for (uint64_t i = 0; i < fd->BlockArray.BlockCount; i++) {
        PBlock block = &fd->BlockArray.Blocks[i];
		__ATRC_FREE_MEMORY_EX(block->Name);
        for (uint64_t j = 0; j < block->KeyArray.KeyCount; j++) {
            PKey key = &block->KeyArray.Keys[j];
            __ATRC_FREE_MEMORY_EX(key->Name);
            __ATRC_FREE_MEMORY_EX(key->Value);
        }
		__ATRC_FREE_MEMORY_EX(block->KeyArray.Keys);
		block->KeyArray.KeyCount = 0;
    }
	__ATRC_FREE_MEMORY_EX(fd->BlockArray.Blocks);
	fd->BlockArray.BlockCount = 0;
}


void Destroy_ATRC_FD(PATRC_FD fd) {
	Destroy_ATRC_FD_Variables(fd);
    Destroy_ATRC_FD_Blocks_And_Keys(fd);
    __ATRC_FREE_MEMORY_EX(fd->Filename);
    ATRC_FREE_MEMORY(fd);
}

void ATRC_FREE_MEMORY(void *ptr) {
    if (ptr == NULL) {
        return;
    }
    free(ptr);
}

PATRC_FD Copy_ATRC_FD(PATRC_FD fd) {
    PATRC_FD copy = Create_Empty_ATRC_FD();
    if (copy == NULL) {
        return NULL;
    }
    copy->AutoSave = fd->AutoSave;
    copy->Writecheck = fd->Writecheck;
    copy->Filename = fd->Filename ? __STRDUP(fd->Filename) : NULL;
    if(copy->Filename == NULL) {
        Destroy_ATRC_FD(copy);
        return NULL;
    }

    copy->BlockArray.BlockCount = fd->BlockArray.BlockCount;
    copy->BlockArray.Blocks = (PBlock)malloc(copy->BlockArray.BlockCount * sizeof(Block));
    if (copy->BlockArray.Blocks == NULL) {
        _ATRC_WRAP_ERRORMSG(ERR_MEMORY_ALLOCATION_FAILED, __LINE__, "Block array allocating error", copy->Filename);
        Destroy_ATRC_FD(copy);
        return NULL;
    }
    for(uint64_t i = 0; i < copy->BlockArray.BlockCount; i++) {
        PBlock src_block = &fd->BlockArray.Blocks[i];
        PBlock dest_block = &copy->BlockArray.Blocks[i];
        dest_block->Name = src_block->Name ? __STRDUP(src_block->Name) : NULL;
        if (dest_block->Name == NULL && src_block->Name != NULL) {
            Destroy_ATRC_FD(copy);
            return NULL;
        }
        dest_block->line_number = src_block->line_number;
        dest_block->KeyArray.KeyCount = src_block->KeyArray.KeyCount;
        dest_block->KeyArray.Keys = (PKey)malloc(dest_block->KeyArray.KeyCount * sizeof(Key));
        if (dest_block->KeyArray.Keys == NULL) {
            _ATRC_WRAP_ERRORMSG(ERR_MEMORY_ALLOCATION_FAILED, __LINE__, "Key array allocating error", copy->Filename);
            Destroy_ATRC_FD(copy);
            return NULL;
        }
        for (uint64_t j = 0; j < dest_block->KeyArray.KeyCount; j++) {
            PKey src_key = &src_block->KeyArray.Keys[j];
            PKey dest_key = &dest_block->KeyArray.Keys[j];
            dest_key->Name = src_key->Name ? __STRDUP(src_key->Name) : NULL;
            if (dest_key->Name == NULL && src_key->Name != NULL) {
                Destroy_ATRC_FD(copy);
                return NULL;
            }
            dest_key->Value = src_key->Value ? __STRDUP(src_key->Value) : NULL;
            if (dest_key->Value == NULL && src_key->Value != NULL) {
                Destroy_ATRC_FD(copy);
                return NULL;
            }
            dest_key->line_number = src_key->line_number;
            dest_key->enum_value = src_key->enum_value;
        }
    }

    copy->VariableArray.VariableCount = fd->VariableArray.VariableCount;
    copy->VariableArray.Variables = (PVariable)malloc(copy->VariableArray.VariableCount * sizeof(Variable));
    if (copy->VariableArray.Variables == NULL) {
        _ATRC_WRAP_ERRORMSG(ERR_MEMORY_ALLOCATION_FAILED, __LINE__, "Variable array allocating error", copy->Filename);
        Destroy_ATRC_FD(copy);
        return NULL;
    }

    for (uint64_t i = 0; i < copy->VariableArray.VariableCount; i++) {
        PVariable src_var = &fd->VariableArray.Variables[i];
        PVariable dest_var = &copy->VariableArray.Variables[i];
        dest_var->Name = src_var->Name ? __STRDUP(src_var->Name) : NULL;
        if (dest_var->Name == NULL && src_var->Name != NULL) {
            Destroy_ATRC_FD(copy);
            return NULL;
        }
        dest_var->Value = src_var->Value ? __STRDUP(src_var->Value) : NULL;
        if (dest_var->Value == NULL && src_var->Value != NULL) {
            Destroy_ATRC_FD(copy);
            return NULL;
        }
        dest_var->IsPublic = src_var->IsPublic;
        dest_var->line_number = src_var->line_number;
    }
    
    return copy;
}