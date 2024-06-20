#include "../include/ATRC.h"
#include "../include/filer.h"

 bool DoesExistVariable(ATRCFiledata* filedata, const std::string& varname){
    for(Variable var : *filedata->Variables){
        if(var.Name == varname){
            if(var.IsPublic) return true;
            else{
                errormsg(ERR_UNAUTHORIZED_ACCESS_TO_VAR, -1, varname);
                return false;            
            }
        } 
    }
    return false;
}

 bool DoesExistKey(ATRCFiledata* filedata, const std::string& block, const std::string& key){
    for(Block blk : *filedata->Blocks){
        if(blk.Name == block){
            for(Key k : blk.Keys){
                if(k.Name == key) return true;
            }
        }
    }
    return false;
}

 bool DoesExistBlock(ATRCFiledata* filedata, const std::string& block){
    for(Block blk : *filedata->Blocks){
        if(blk.Name == block) return true;
    }
    return false;
}
