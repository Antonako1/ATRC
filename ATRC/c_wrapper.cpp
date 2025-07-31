#define _CRT_SECURE_NO_WARNINGS
#include "./include/ATRC.h"
#include "./include/filer.h"
#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <fstream>
#include <cstdlib>
/*+++
Wrap around functions from C++ to c
---*/

/*_ATRC_WRAP_READ*/
bool _ATRC_WRAP_READ(PATRC_FD fd, const char* path, ReadMode readMode) {
    std::string filename = path;
    std::string encoding = "UTF-8";
    std::string extension = "atrc";
        if (readMode == ATRC_FORCE_READ) {
        // Delete previous file and create a new one
        std::ofstream ofs(filename, std::ios::out | std::ios::trunc);
        if (ofs.is_open()) {
            ofs << "#!ATRC" << "\n";
        } else {
            errormsg(FILE_MODE_ERR, -1, filename, filename);
            return false;
        }
    } else if (readMode == ATRC_CREATE_READ) {
        std::ifstream ifs(filename);
        if (!ifs.good()) {
            std::ofstream ofs(filename, std::ios::out | std::ios::trunc);
            if (ofs.is_open()) {
                ofs << "#!ATRC" << "\n";
            } else {
                errormsg(FILE_MODE_ERR, -1, filename, filename);
                return false;
            }
        }
    }
    std::vector<CPP_Variable> variables;
    std::vector<CPP_Block> blocks;
    auto parsedData = ParseFile(filename, encoding, extension, variables, blocks);
    if (!parsedData) {
        errormsg(ERR_INVALID_FILE, -1, filename, filename);
        return false;
    }

    // Cleanup existing data
    Destroy_ATRC_FD_Blocks_And_Keys(fd);
    Destroy_ATRC_FD_Variables(fd);

    // Copy new data
    size_t var_amount = variables.size();
    
    fd->VariableArray.VariableCount = (uint64_t)var_amount;
    fd->VariableArray.Variables = (Variable*)malloc(var_amount * sizeof(Variable));
    if(fd->VariableArray.Variables == NULL) {
        errormsg(ERR_MEMORY_ALLOCATION_FAILED, __LINE__, "Variable array allocating error", filename);
        return false;
	}

    size_t counter = 0;
    for(const auto &var : variables) {
        Variable c_var{
            __STRDUP(var.Name.c_str()),
            __STRDUP(var.Value.c_str()),
			var.IsPublic,
			var.line_number
        };
		fd->VariableArray.Variables[counter] = c_var;
        counter++;
    }

    size_t block_amount = blocks.size();
	fd->BlockArray.BlockCount = (uint64_t)block_amount;
	fd->BlockArray.Blocks = (Block*)malloc(block_amount * sizeof(Block));
    if(fd->BlockArray.Blocks == NULL) {
        errormsg(ERR_MEMORY_ALLOCATION_FAILED, __LINE__, "Block array allocating error", filename);
        Destroy_ATRC_FD_Variables(fd);
        return false;
	}
    
    for (size_t i = 0; i < blocks.size(); ++i) {
        const auto& block = blocks[i];
        size_t keys_amount = block.Keys.size();

        Block c_block{
            __STRDUP(block.Name.c_str()),
            {
                (Key*)calloc(keys_amount, sizeof(Key)),
                (uint64_t)keys_amount
            },
            block.line_number
        };

        if (!c_block.Name || !c_block.KeyArray.Keys) {
            errormsg(ERR_MEMORY_ALLOCATION_FAILED, __LINE__, "Block allocation failed", filename);
            Destroy_ATRC_FD_Variables(fd);
            Destroy_ATRC_FD_Blocks_And_Keys(fd);
            return false;
        }

        for (size_t j = 0; j < keys_amount; ++j) {
            const auto& key = block.Keys[j];
            Key c_key{
                __STRDUP(key.Name.c_str()),
                __STRDUP(key.Value.c_str()),
                key.line_number,
                key.enum_value
            };

            if (!c_key.Name || !c_key.Value) {
                errormsg(ERR_MEMORY_ALLOCATION_FAILED, __LINE__, "Key strdup failed", filename);
                Destroy_ATRC_FD_Variables(fd);
                Destroy_ATRC_FD_Blocks_And_Keys(fd);
                return false;
            }

            c_block.KeyArray.Keys[j] = c_key;
        }

        fd->BlockArray.Blocks[i] = c_block;
    }

    return true;
}

/*_ATRC_WRAP_ERRORMSG*/
void _ATRC_WRAP_ERRORMSG(int err_num, int line_number, const char *var_name, const char *filename){
    errormsg(err_num, line_number, var_name, filename);
}

/*_ATRC_WRAP__W_SAVE*/
void _ATRC_WRAP__W_SAVE(PATRC_FD self, const size_t action, const size_t xtra_info, const char *varname, const char *xtra_info4,const char *xtra_info5){
    CPP_ATRC_FD fd(self);

    if((ATRC_SAVE)action == ATRC_SAVE::ADD_VAR){
        std::string temp1 = varname;
        std::string temp2 = xtra_info4;
        _W_Save_(&fd, (ATRC_SAVE)action, (int)xtra_info, "%"+temp1+"%="+ParseLineSTRINGtoATRC(temp2));
    } 
    else if((ATRC_SAVE)action == ATRC_SAVE::MODIFY_VAR){
        std::string temp1 = varname;
        _W_Save_(&fd, (ATRC_SAVE)action, (int)xtra_info, ParseLineSTRINGtoATRC(varname));
    }
    else {
        std::string varname_str = varname == NULL ? std::string() : std::string(varname);
        std::string xtra_info4_str = xtra_info4 == NULL ? std::string() : std::string(xtra_info4);
        std::string xtra_info5_str = xtra_info5 == NULL ? std::string() : std::string(xtra_info5);
        _W_Save_(&fd, (ATRC_SAVE)action, (int)xtra_info, varname_str, xtra_info4_str, xtra_info5_str);
    }
    fd.SetAutoSave(false);
    return;
}

/* INSERT_VAR */
void _ATRC_WRAP_INSERTVAR(char* line, const char** args){
    CPP_ATRC_FD fd = CPP_ATRC_FD();
    std::vector<std::string> args_v;
    for(uint64_t i = 0; args[i] != NULL; i++){
        args_v.push_back(args[i]);
    }
    std::string line_s = line;
    fd.InsertVar(line_s, args_v);
    size_t line_size = strlen(line);
    strncpy(line, line_s.c_str(), line_size - 1);
    line[line_size - 1] = '\0';
}

/* INSERT_VAR_S */
char* _ATRC_WRAP_INSERTVAR_S(const char* line, const char** args) {
    if(line == NULL || args == NULL) {
        return NULL;
	}
    CPP_ATRC_FD fd = CPP_ATRC_FD();
    std::vector<std::string> args_v;
    for (uint64_t i = 0; args[i] != NULL; i++) {
        args_v.push_back(args[i]);
    }
    std::string line_s = std::string(line);
    std::string res = fd.InsertVar_S(line_s, args_v);

    if (res.empty()) {
        return NULL;
    }

    char *cstr = __STRDUP(res.c_str());
    if (cstr == NULL) {
        return NULL; // Memory allocation failed
    }
    
    return cstr;
}

