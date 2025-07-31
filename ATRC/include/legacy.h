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

/*
Contains legacy c++ definitions
*/
#pragma once
#ifndef LEGACY_ATRC_H
#define LEGACY_ATRC_H

#ifdef __cplusplus
#include <string>
#include <vector>
#include <stdint.h>
#include <ATRC.h>

typedef struct _CPP_Variable {
    std::string Name;
    std::string Value;
    bool IsPublic = true;
    uint64_t line_number = (uint64_t)-1;
} CPP_Variable, * CPP_PVariable;

typedef struct _CPP_Key {
    std::string Name;
    std::string Value;
    uint64_t line_number = (uint64_t)-1;
    uint64_t enum_value = (uint64_t)-1;
} CPP_Key, * CPP_PKey;


typedef struct _CPP_Block {
    std::string Name;
    std::vector<CPP_Key> Keys;
    uint64_t line_number = (uint64_t)-1;
} CPP_Block, * CPP_PBlock;

class PROXY_ATRC_FD;

class CPP_ATRC_FD {
public:
    CPP_ATRC_FD();
    CPP_ATRC_FD(std::string& path, ReadMode mode = ATRC_READ_ONLY);
    CPP_ATRC_FD(const char *path, ReadMode mode = ATRC_READ_ONLY);
    CPP_ATRC_FD(PATRC_FD filedata);
    ~CPP_ATRC_FD();
    bool ReadAgain(ReadMode mode = ATRC_READ_ONLY);
    bool Read(std::string& path, ReadMode mode = ATRC_READ_ONLY);
    std::string ReadVariable(const std::string& varname);
    std::string ReadKey(const std::string& block, const std::string& key);
    bool DoesExistBlock(const std::string& block);
    bool DoesExistVariable(const std::string& varname);
    bool DoesExistKey(const std::string& block, const std::string& key);
    bool IsPublic(const std::string& varname);
    void InsertVar(std::string& line, std::vector<std::string>& args);
    std::string InsertVar_S(const std::string& line, std::vector<std::string>& args);
    bool AddBlock(const std::string& blockname);
    bool RemoveBlock(const std::string& blockname);
    bool AddVariable(const std::string& varname, const std::string& value);
    bool RemoveVariable(const std::string& varname);
    bool ModifyVariable(const std::string& varname, const std::string& value);
    bool AddKey(const std::string& block, const std::string& key, const std::string& value);
    bool RemoveKey(const std::string& block, const std::string& key);
    bool ModifyKey(const std::string& block, const std::string& key, const std::string& value);
    bool WriteCommentToTop(const std::string& comment);
    bool WriteCommentToBottom(const std::string& comment);
    PATRC_FD ToCStruct();

    uint64_t GetEnumValue(const std::string& block, const std::string& key);

    bool CheckStatus();

    std::vector<CPP_Variable> GetVariables();
    std::vector<CPP_Block>* GetBlocks();
    std::string GetFilename();
    bool GetAutoSave() const;
    void SetAutoSave(bool autosave);
    bool GetWriteCheck() const;
    void SetWriteCheck(bool writecheck);
    
    PROXY_ATRC_FD operator[](const std::string& key);
    PROXY_ATRC_FD operator[](const std::string& key) const;

    std::vector<CPP_Variable> Variables;
    std::vector<CPP_Block> Blocks;
private:
    void MAINCONSTRUCTOR();
    bool AutoSave;
    bool Writecheck;
    std::string Filename;
    bool safeToUse;
};
typedef CPP_ATRC_FD* CPP_PATRC_FD;

class PROXY_ATRC_FD {
public:
    PROXY_ATRC_FD(CPP_ATRC_FD& fd, const std::string& key);
    PROXY_ATRC_FD operator[](const std::string& subKey);
    operator std::string() const;
    operator const char*() const;
    PROXY_ATRC_FD& operator=(const std::string& value);    
    PROXY_ATRC_FD& operator>>(const std::string& value);
    PROXY_ATRC_FD& operator>>(const char* value);

	inline friend std::ostream& operator<<(std::ostream& os, const PROXY_ATRC_FD& obj) {
        uint64_t x = obj.key.find("]");
        if(x == std::string::npos) os << obj.fd->ReadVariable(obj.key);
        else {
            std::string block = obj.key.substr(0, x);
            std::string key_ = obj.key.substr(x + 1, obj.key.size() - x - 1);
            os << obj.fd->ReadKey(block, key_);
        }
		return os;
	}
private:
    CPP_PATRC_FD fd;
    std::string key;
};

#endif // __cplusplus
#endif // LEGACY_ATRC_H