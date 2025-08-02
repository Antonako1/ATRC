
#include "./include/ATRC/ATRC.h"
#include "./include/filer.h"
#include <iostream>
#include <string>
#include <vector>

bool BlockContainsKey(std::vector<CPP_Key>& keys, const CPP_Key& key) {
    for (const CPP_Key& _key : keys) {
        if (_key.Name == key.Name) {
            return true;
        }
    }
    return false;
}
bool BlockContainsBlock(std::vector<CPP_Block>& blocks,const CPP_Block& block) {
    for (const CPP_Block& _block : blocks) {
        if (_block.Name == block.Name) {
            return true;
        }
    }
    return false;
}
bool VariableContainsVariable(std::vector<CPP_Variable>& variables, const CPP_Variable& variable){
    for (const CPP_Variable& var : variables) {
        if (var.Name == variable.Name) {
            return true;
        }
    }
    return false;
}