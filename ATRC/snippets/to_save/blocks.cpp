#include "../../include/ATRC.hpp"
#include "../../include/filer.h"
#include <string>
#include <memory>
/*
    ! TEST EVERYTHING
*/
 void AddBlock(std::shared_ptr<ATRC_FD> filedata, const std::string& blockname) {
    Block block;
    block.Name = blockname;
    if(BlockContainsBlock(filedata->Blocks, block)) {
        errormsg(ERR_BLOCK_EXISTS, -1, block.Name, filedata->Filename);
        return;
    }
    filedata->Blocks->push_back(block);

    if(filedata->AutoSave) {
        Save(filedata, ATRC_SAVE::ADD_BLOCK, -1, blockname);
    }
}

 void RemoveBlock(std::shared_ptr<ATRC_FD> filedata, const std::string& blockname) {
    Block block;
    block.Name = blockname;
    if(!BlockContainsBlock(filedata->Blocks, block)) {
        errormsg(ERR_BLOCK_NOT_FOUND, -1, blockname, filedata->Filename);
        return;
    }

    int i = 0;
    for(Block blk : *filedata->Blocks) {
        if(blk.Name == block.Name) break;
        i++;
    }
    filedata->Blocks->erase(filedata->Blocks->begin() + i);
    if(filedata->AutoSave) {
        Save(filedata, ATRC_SAVE::REMOVE_BLOCK, i, blockname);
    }
}