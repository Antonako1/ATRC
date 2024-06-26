#include "include/filer.h"
#include "include/ATRC.h"
// #include <string>
#include <vector>
#include <memory>

bool BlockContainsKey(std::vector<Key>& keys, Key* key) {
    for (const Key& _key : keys) {
        if (_key.Name == key->Name) {
            return true;
        }
    }
    return false;
}


bool BlockContainsBlock(std::unique_ptr<std::vector<Block>>& blocks,Block* block) {
    for (Block _block : *blocks) {
        if (_block.Name == block->Name) {
            return true;
        }
    }
    return false;
}

bool VariableContainsVariable(std::unique_ptr<std::vector<Variable>>& variables,Variable* variable){
    for (Variable var : *variables) {
        if (var.Name == variable->Name) {
            return true;
        }
    }
    return false;
}