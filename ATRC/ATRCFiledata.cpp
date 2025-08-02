/*+++
NOTE: This file is used only internaly as of now.
---*/

#define _CRT_SECURE_NO_DEPRECATE
#include "./include/ATRC/ATRC.h"
#include "./include/filer.h"
#include <iostream>
#include <cstring>
#include <fstream>
#include <cstdlib>
#include <legacy.h>

void CPP_ATRC_FD::MAINCONSTRUCTOR(){
    this->AutoSave = false;
    this->Writecheck = false;
    this->Filename = "";
    this->Variables = std::vector<CPP_Variable>(); 
    this->Blocks = std::vector<CPP_Block>();
}

CPP_ATRC_FD::CPP_ATRC_FD(){this->MAINCONSTRUCTOR();}

CPP_ATRC_FD::CPP_ATRC_FD(const char *path, ReadMode mode){
    std::string _path = path;
    this->MAINCONSTRUCTOR();
    this->Filename = _path;
    if (!this->ReadAgain(mode)) {
        errormsg(ERR_INVALID_FILE, -1, "Failed to read file: " + std::string(path), path);
    }

    
}
CPP_ATRC_FD::CPP_ATRC_FD(std::string& path, ReadMode mode){
    this->MAINCONSTRUCTOR();
    this->Filename = path;
    if (!this->ReadAgain(mode)) {
        errormsg(ERR_INVALID_FILE, -1, "Failed to read file: " + path, path);
    };
}

CPP_ATRC_FD::CPP_ATRC_FD(PATRC_FD fd){
	this->MAINCONSTRUCTOR();
	this->Filename = std::string(fd->Filename);
	this->AutoSave = fd->AutoSave;
	this->Writecheck = fd->Writecheck;
    this->Variables = std::vector<CPP_Variable>();
    for(uint64_t i = 0; i < fd->VariableArray.VariableCount; i++) {
        CPP_Variable var;
        var.Name = std::string(fd->VariableArray.Variables[i].Name);
        var.Value = std::string(fd->VariableArray.Variables[i].Value);
        var.IsPublic = fd->VariableArray.Variables[i].IsPublic;
        var.line_number = fd->VariableArray.Variables[i].line_number;
        this->Variables.push_back(var);
	}

    this->Blocks = std::vector<CPP_Block>();
    for (uint64_t i = 0; i < fd->BlockArray.BlockCount; i++) {
        CPP_Block block;
        block.Name = std::string(fd->BlockArray.Blocks[i].Name);
        block.line_number = fd->BlockArray.Blocks[i].line_number;
        for (uint64_t j = 0; j < fd->BlockArray.Blocks[i].KeyArray.KeyCount; j++) {
            CPP_Key key;
            key.Name = std::string(fd->BlockArray.Blocks[i].KeyArray.Keys[j].Name);
            key.Value = std::string(fd->BlockArray.Blocks[i].KeyArray.Keys[j].Value);
            key.line_number = fd->BlockArray.Blocks[i].KeyArray.Keys[j].line_number;
            key.enum_value = fd->BlockArray.Blocks[i].KeyArray.Keys[j].enum_value;
            block.Keys.push_back(key);
        }
        this->Blocks.push_back(block);
    }
	this->safeToUse = true;
}

PATRC_FD CPP_ATRC_FD::ToCStruct() {
	PATRC_FD fd = Create_Empty_ATRC_FD();

	fd->AutoSave = this->AutoSave;
	fd->Writecheck = this->Writecheck;
	fd->Filename = __STRDUP(this->Filename.c_str());

	fd->VariableArray.VariableCount = (uint64_t)this->Variables.size();
	fd->VariableArray.Variables = (PVariable)malloc((size_t)fd->VariableArray.VariableCount * sizeof(Variable));
	if (fd->VariableArray.Variables == NULL) {
		errormsg(ERR_MEMORY_ALLOCATION_FAILED, __LINE__, "Variable array allocating error", fd->Filename);
		Destroy_ATRC_FD(fd);
        fd = NULL;
		return NULL;
	}
	size_t counter = 0, counter2 = 0;
	for (const auto& var : this->Variables) {
		fd->VariableArray.Variables[counter].Name = __STRDUP(var.Name.c_str());
		fd->VariableArray.Variables[counter].Value = __STRDUP(var.Value.c_str());
		fd->VariableArray.Variables[counter].IsPublic = var.IsPublic;
		fd->VariableArray.Variables[counter].line_number = var.line_number;
		counter++;
	}

	counter = 0, counter2 = 0;
	fd->BlockArray.BlockCount = (uint64_t)this->Blocks.size();
	fd->BlockArray.Blocks = (PBlock)malloc((size_t)fd->BlockArray.BlockCount * sizeof(Block));
	if (fd->BlockArray.Blocks == NULL) {
		errormsg(ERR_MEMORY_ALLOCATION_FAILED, __LINE__, "Block array allocating error", fd->Filename);
		Destroy_ATRC_FD(fd);
        fd = NULL;
        return NULL;
	}

    for(const auto& block : this->Blocks) {
        size_t keys_amount = block.Keys.size();
        fd->BlockArray.Blocks[counter].Name = __STRDUP(block.Name.c_str());
        fd->BlockArray.Blocks[counter].line_number = block.line_number;
        fd->BlockArray.Blocks[counter].KeyArray.KeyCount = (uint64_t)keys_amount;
        fd->BlockArray.Blocks[counter].KeyArray.Keys = (PKey)malloc(keys_amount * sizeof(Key));
        if(fd->BlockArray.Blocks[counter].KeyArray.Keys == NULL) {
            errormsg(ERR_MEMORY_ALLOCATION_FAILED, __LINE__, "Key array allocating error", fd->Filename);
            Destroy_ATRC_FD(fd);
            fd = NULL;
            return NULL;
        }
        for(const auto &key : block.Keys) {
            fd->BlockArray.Blocks[counter].KeyArray.Keys[counter2].Name = __STRDUP(key.Name.c_str());
            fd->BlockArray.Blocks[counter].KeyArray.Keys[counter2].Value = __STRDUP(key.Value.c_str());
            fd->BlockArray.Blocks[counter].KeyArray.Keys[counter2].line_number = key.line_number;
            fd->BlockArray.Blocks[counter].KeyArray.Keys[counter2].enum_value = key.enum_value;
            counter2++;
        }
        counter2 = 0;
        counter++;
	}

    return fd;
}

CPP_ATRC_FD::~CPP_ATRC_FD(){
    this->Variables.clear();
    this->Blocks.clear();
}

PROXY_ATRC_FD CPP_ATRC_FD::operator[](const std::string& key) {
    return PROXY_ATRC_FD(*this, key);
}

PROXY_ATRC_FD CPP_ATRC_FD::operator[](const std::string& key) const {
    return PROXY_ATRC_FD(const_cast<CPP_ATRC_FD&>(*this), key);
}


bool CPP_ATRC_FD::Read(std::string& path, ReadMode mode){
    this->Filename = path;
    return this->ReadAgain(mode);
}

bool CPP_ATRC_FD::ReadAgain(ReadMode mode){
    std::string filename = this->Filename;
    std::string encoding = "UTF-8";
    std::string extension = "atrc";
    if (mode == ATRC_FORCE_READ) {
        // Delete previous file and create a new one
        std::ofstream ofs(filename, std::ios::out | std::ios::trunc);
        if (ofs.is_open()) {
            ofs << "#!ATRC" << "\n";
        } else {
            errormsg(FILE_MODE_ERR, -1, filename, filename);
            this->safeToUse = false;
            return false;
        }
    } else if (mode == ATRC_CREATE_READ) {
        std::ifstream ifs(filename);
        if (!ifs.good()) {
            std::ofstream ofs(filename, std::ios::out | std::ios::trunc);
            if (ofs.is_open()) {
                ofs << "#!ATRC" << "\n";
            } else {
                errormsg(FILE_MODE_ERR, -1, filename, filename);
                this->safeToUse = false;
                return false;
            }
        }
    }

    bool parsedData = ParseFile(filename, encoding, extension, this->Variables, this->Blocks);
    if (!parsedData) {
        this->safeToUse = false;
        return false;
    }
    this->safeToUse = true;
    return true;
}




std::vector<CPP_Variable> CPP_ATRC_FD::GetVariables(){
    std::vector<CPP_Variable> _vars = std::vector<CPP_Variable>();
    for(CPP_Variable &var : this->Variables){
        if(var.IsPublic) _vars.push_back(var);
    }
    return _vars;
}
std::vector<CPP_Block>* CPP_ATRC_FD::GetBlocks(){
    return &this->Blocks;
}
std::string CPP_ATRC_FD::GetFilename(){
    return this->Filename;
}
bool CPP_ATRC_FD::GetAutoSave() const {
    return this->AutoSave;
}
void CPP_ATRC_FD::SetAutoSave(bool autosave){
    this->AutoSave = autosave;
}

bool CPP_ATRC_FD::GetWriteCheck() const {
    return this->Writecheck;
}
void CPP_ATRC_FD::SetWriteCheck(bool writecheck) {
    this->Writecheck = writecheck;
}

std::string CPP_ATRC_FD::ReadVariable(const std::string& varname){
    std::string res = "";
    for(CPP_Variable var : this->Variables){
        if(var.Name == varname){
            if(var.IsPublic) {
                res = var.Value;
            } else {
                errormsg(ERR_UNAUTHORIZED_ACCESS_TO_VAR, -1, varname, this->Filename);
                res = "";
            }
            return res;
        }
    }
    res = "";
    return res;
}
std::string CPP_ATRC_FD::ReadKey(const std::string& block, const std::string& key){
    std::string res = "";
    for(CPP_Block blk : this->Blocks){
        if(blk.Name == block){
            for(CPP_Key k : blk.Keys){
                if(k.Name == key){
                    res = k.Value;
                    return res;
                }
            }
        }
    }
    res = "";
    return res;
}
std::string CPP_ATRC_FD::InsertVar_S(const std::string &line, std::vector<std::string> &args){
    std::string res = line;
    InsertVar(res, args);
    return res;
}
bool CPP_ATRC_FD::DoesExistBlock(const std::string& block){
    for(CPP_Block &blk : this->Blocks){
        if(blk.Name == block) return true;
    }
    return false;
}
bool CPP_ATRC_FD::DoesExistVariable(const std::string& varname){
    for(CPP_Variable& var : this->Variables){
        if(var.Name == varname){
            if(var.IsPublic) return true;
            else{
                errormsg(ERR_UNAUTHORIZED_ACCESS_TO_VAR, -1, varname, this->Filename);
                return false;
            }
        } 
    }
    return false;
}

bool CPP_ATRC_FD::CheckStatus() {
    return this->safeToUse;
}

bool CPP_ATRC_FD::DoesExistKey(const std::string& block, const std::string& key){
    for(CPP_Block &blk : this->Blocks){
            if(blk.Name == block){
                for(CPP_Key &k : blk.Keys){
                    if(k.Name == key) return true;
                }
            }
        }
        return false;
}
bool CPP_ATRC_FD::IsPublic(const std::string& varname){
    for(CPP_Variable &var : this->Variables){
        if(var.Name == varname){
            return var.IsPublic;
        } 
    }
    return false;
}
void CPP_ATRC_FD::InsertVar(std::string &line, std::vector<std::string> &args){
    bool _looking_for_var = false;
    std::string var;
    std::string _result;
    int _arg_counter = 0;

    for (size_t i = 0; i < line.length(); ++i) {
        if (line[i] == '\0') break;
        if (line[i] == '%' || _looking_for_var) {
            var += line[i];
            if (line[i] == '%') {
                if (_looking_for_var) {
                    if (var.substr(0, 2) == "%*") {
                        if(var == "%*%"){
                            _result += args[(size_t)_arg_counter++];
                        } else {
                            // check %*<digit>%
                            if(var.size() > 3) {
                                // size_t inject_num = std::stoull(var.substr(2, var.size() - 3));
                                std::string inject_num_str = var.substr(2, var.size() - (3+2)); // digits up to %*xxxx*
                                size_t injectables = 0;
                                for(char c : inject_num_str) {
                                    if (!isdigit(c)) {
                                        return;
                                    }
                                    injectables++;
                                }
                                
                                size_t inject_num = std::stoi(var.substr(2, var.size() - (3+injectables))); 

                                if(args.size() > inject_num){
                                    _result += args[inject_num];
                                } else {
                                    errormsg(ERR_INSERT_WRONG, -1, var + " inject: " + std::to_string(inject_num), this->Filename);
                                }
                            } else {
                                errormsg(ERR_INSERT_WRONG, -1, var, this->Filename);
                            }

                        }
                    } else {
                        _result += var;
                    }
                    var.clear();
                    _looking_for_var = false;
                } else {
                    _looking_for_var = true;
                }
            }
            continue;
        }
        _result += line[i];
    }
    line = _result;
}
bool CPP_ATRC_FD::AddBlock(const std::string& blockname){
    if(DoesExistBlock(blockname)){
        errormsg(ERR_BLOCK_EXISTS, -1, blockname, this->Filename);
        return false;
    }
    CPP_Block blk;
    blk.Name = blockname;
    this->Blocks.push_back(blk);
    if(this->AutoSave){
        _W_Save_(this, ATRC_SAVE::ADD_BLOCK, -1, blockname);
    }
    return true;
}
bool CPP_ATRC_FD::RemoveBlock(const std::string& blockname){
    CPP_Block block;
    block.Name = blockname;
    if(!BlockContainsBlock(this->Blocks, block)) {
        errormsg(ERR_BLOCK_NOT_FOUND, -1, blockname, this->Filename);
        return false;
    }

    int i = 0;
    for(CPP_Block blk : this->Blocks) {
        if(blk.Name == block.Name) break;
        i++;
    }
    this->Blocks.erase(this->Blocks.begin() + i);
    if(this->AutoSave) {
        _W_Save_(this, ATRC_SAVE::REMOVE_BLOCK, i, blockname);
    }
    return true;
}
bool CPP_ATRC_FD::AddVariable(const std::string& varname, const std::string& value){
    CPP_Variable var;
    var.Name = varname;
    if(DoesExistVariable(varname)){
        errormsg(ERR_VAR_EXISTS, -1, var.Name, this->Filename);
        return false;
    }
    var.Value = value;
    this->Variables.push_back(var);
    if(this->AutoSave){
        _W_Save_(this, ATRC_SAVE::ADD_VAR, -1, "%"+var.Name+"%="+ParseLineSTRINGtoATRC(var.Value));
    }
    return true;
}
bool CPP_ATRC_FD::RemoveVariable(const std::string& varname){
    CPP_Variable var;
    var.Name = varname;
    if(!DoesExistVariable(varname)){
        errormsg(ERR_VAR_NOT_FOUND, -1, var.Name, this->Filename);
        return false;
    }

    int i = 0;    
    for(CPP_Variable var : this->Variables){
        if(var.Name == varname){
            break;
        }
        i++;
    }
    this->Variables.erase(this->Variables.begin() + i);

    if(this->AutoSave){
        _W_Save_(this, ATRC_SAVE::REMOVE_VAR, i, varname);
    }
    return true;
}
bool CPP_ATRC_FD::ModifyVariable(const std::string& varname, const std::string& value){
    CPP_Variable var;
    var.Name = varname;
    if(!DoesExistVariable(varname)){
        if(!this->Writecheck) {
            errormsg(ERR_VAR_NOT_FOUND, -1, var.Name, this->Filename);
            return false;
        }
        if(!this->AddVariable(varname, value)){
            errormsg(ERR_WRITECHECK, -1, var.Name, this->Filename);
            return false;
        }
    }
    int i = 0;    
    for(CPP_Variable &var : this->Variables){
        if(var.Name == varname){
            break;
        }
        i++;
    }
    this->Variables.at((size_t)i).Value = value;
    if(this->AutoSave){
        _W_Save_(this, ATRC_SAVE::MODIFY_VAR, i, ParseLineSTRINGtoATRC(value)); // Values taken from filedata->Variables->at(xtra_info).[type]
    } 
    return true;
}
bool CPP_ATRC_FD::AddKey(const std::string& block, const std::string& key, const std::string& value){
    if(!DoesExistBlock(block)) {
        errormsg(ERR_BLOCK_NOT_FOUND, -1, block, this->Filename);
        return false;
    }
    if(DoesExistKey(block, key)){
        errormsg(ERR_KEY_EXISTS, -1, key, this->Filename);
        return false;
    }

    CPP_Key _key;
    _key.Name = key;
    _key.Value = value;
    for(CPP_Block &_block : this->Blocks){
        if(_block.Name == block){
            _block.Keys.push_back(_key);
            break;
        }
    }
    if(this->AutoSave){
        _W_Save_(this, ATRC_SAVE::ADD_KEY, -1, key, value, block);
    }
    return true;
}
bool CPP_ATRC_FD::RemoveKey(const std::string& block, const std::string& key){
    if(!DoesExistBlock(block)) {
        errormsg(ERR_BLOCK_NOT_FOUND, -1, block, this->Filename);
        return false;
    }
    if(!DoesExistKey(block, key)){
        errormsg(ERR_KEY_NOT_FOUND, -1, key, this->Filename);
        return false;
    }
    int i = 0;
    for (CPP_Block &_block : this->Blocks) {
        if (_block.Name == block) {
            auto it = _block.Keys.begin();
            while (it != _block.Keys.end()) {
                if (it->Name == key) {
                    it = _block.Keys.erase(it);
                } else {
                    ++it;
                    i++;
                }
            }
            break;
        }
    }


    if(this->AutoSave){
        _W_Save_(this, ATRC_SAVE::REMOVE_KEY, i, key, "", block);
    }
    return true;
}
bool CPP_ATRC_FD::ModifyKey(const std::string& block, const std::string& key, const std::string& value){
    if(!DoesExistBlock(block)) {
        if(!this->Writecheck){
            errormsg(ERR_BLOCK_NOT_FOUND, -1, block, this->Filename);
            return false;
        } else {
            if(!this->AddBlock(block)){
                errormsg(ERR_WRITECHECK, -1, block, this->Filename);
                return false;
            }
        }
    }
    if(!DoesExistKey(block, key)){
        if(!this->Writecheck){
            errormsg(ERR_KEY_NOT_FOUND, -1, key, this->Filename);
            return false;
        } else {
            if(!this->AddKey(block, key, value)){
                errormsg(ERR_WRITECHECK, -1, key, this->Filename);
                return false;
            }
        }
    }

    int i = 0;
    for(CPP_Block &_blk : this->Blocks){
        if(_blk.Name == block){
            for(CPP_Key &_key : _blk.Keys){
                if(_key.Name == key){
                    _key.Value = value;
                    break;
                }
                i++;
            }
            break;
        }
    }

    if(this->AutoSave){
        _W_Save_(this, ATRC_SAVE::MODIFY_KEY, i, key, value, block);
    }
    return true;
}

bool CPP_ATRC_FD::WriteCommentToTop(const std::string& comment) {
    if (this->AutoSave) {
        _W_Save_(this, ATRC_SAVE::WRITE_COMMENT_TO_TOP, -1, comment);
        return true;
    }
    return false;
}
bool CPP_ATRC_FD::WriteCommentToBottom(const std::string& comment) {
    if (this->AutoSave) {
        _W_Save_(this, ATRC_SAVE::WRITE_COMMENT_TO_BOTTOM, -1, comment);
        return true;
    }
    return false;
}

uint64_t CPP_ATRC_FD::GetEnumValue(const std::string& block, const std::string& key) {
    for (const CPP_Block& blk : this->Blocks) {
        if (blk.Name == block) {
            for (const CPP_Key& k : blk.Keys) {
                if (k.Name == key) {
                    return k.enum_value;
                }
            }
        }
    }
    return (uint64_t)-1; // Return -1 if not found
}

/*+++
PROXY_ATRC_FD
---*/
#include <iostream>
PROXY_ATRC_FD::PROXY_ATRC_FD(CPP_ATRC_FD& fd, const std::string& key) : fd(&fd), key(key) {}

PROXY_ATRC_FD PROXY_ATRC_FD::operator[](const std::string& subKey) {
	std::string combined_key = key + "]" + subKey;
	PROXY_ATRC_FD res = PROXY_ATRC_FD(*fd, combined_key);
    if (res == 0) {
		std::cerr << "312_Error: PROXY_ATRC_FD is null." << std::endl;
    }
	return res;
}

PROXY_ATRC_FD::operator const char*() const {
    size_t x = key.find("]");
    try {
        if (x == std::string::npos) {
            std::string res_str = fd->ReadVariable(key);
            const char *res = (char*)malloc(res_str.size() + 1);
            if (!res) {
                std::cerr << "513_Memory allocation failed for PROXY_ATRC_FD." << std::endl;
                return "";
			}
            std::strcpy(const_cast<char*>(res), res_str.c_str());
            return res;
        }
        else {
            std::string block = key.substr(0, x);
            std::string key_ = key.substr(x + 1, key.size() - x - 1);
            std::string res_str = fd->ReadKey(block, key_);
            if(res_str.empty()) {
                return "";
			}
            const char *res = (char*)malloc(res_str.size() + 1);
            if(!res) {
                std::cerr << "512_Memory allocation failed for PROXY_ATRC_FD." << std::endl;
                return "";
			}
            std::strcpy(const_cast<char*>(res), res_str.c_str());
            return res;
        }
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    return "";
}

PROXY_ATRC_FD::operator std::string() const {
	size_t x = key.find("]");
    try {
		if (x == std::string::npos) {
			return fd->ReadVariable(key);
		}
		else {
			std::string block = key.substr(0, x);
			std::string key_ = key.substr(x + 1, key.size() - x - 1);
			return fd->ReadKey(block, key_);
		}
	}
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    return "";
}
PROXY_ATRC_FD& PROXY_ATRC_FD::operator=(const std::string& value) {
    size_t x = key.find("]");
    try {
        if (x == std::string::npos) {
            if(fd->GetWriteCheck() || fd->DoesExistVariable(key)) {
                fd->ModifyVariable(key, value);
            }
        }
        else {
            std::string block = key.substr(0, x);
            std::string key_ = key.substr(x + 1, key.size() - x - 1);
            if(fd->GetWriteCheck() || fd->DoesExistKey(block, key_)) {
                    fd->ModifyKey(block, key_, value);
            }
        }
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    return *this;
}


PROXY_ATRC_FD& PROXY_ATRC_FD::operator>>(const std::string& value) {
    size_t x = key.find("]");
    if (x == std::string::npos) {
        std::string existing = fd->ReadVariable(key);
        fd->ModifyVariable(key, existing + value);
    } else {
        std::string block = key.substr(0, x);
        std::string key_ = key.substr(x + 1, key.size() - x - 1);
        std::string existing = fd->ReadKey(block, key_);
        fd->ModifyKey(block, key_, existing + value);
    }
    return *this;
}

PROXY_ATRC_FD& PROXY_ATRC_FD::operator>>(const char* value) {
    return operator>>(std::string(value));
}


