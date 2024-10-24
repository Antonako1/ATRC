#include "../include/ATRC.hpp"

 bool IsPublic(std::shared_ptr<ATRC_FD> filedata, const std::string& varname){
    for(Variable var : *filedata->Variables){
        if(var.Name == varname){
            return var.IsPublic;
        } 
    }
    return false;
}