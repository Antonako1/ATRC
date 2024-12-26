#include "../include/ATRC.hpp"
#include "../include/filer.h"
#include <string>
#include <vector>

void _W_InsertVar_(PATRC_FD filedata, std::string &line, std::vector<std::string> &args){
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
                            _result += args[_arg_counter++];
                        } else {
                            
                            // check %*<digit>%
                            if(var.size() > 3) {
                                // size_t inject_num = std::stoull(var.substr(2, var.size() - 3));
                                size_t inject_num = std::stoi(var.substr(2, var.size() - 3));
                                if(args.size() > inject_num){
                                    _result += args[inject_num];
                                } else {
                                    errormsg(ERR_INSERT_WRONG, -1, var + " inject: " + std::to_string(inject_num), filedata->Filename);
                                }
                            } else {
                                errormsg(ERR_INSERT_WRONG, -1, var, filedata->Filename);
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
std::string _W_InsertVar_S_(PATRC_FD filedata, const std::string &line, std::vector<std::string> &args){
    std::string _line = line;
    _W_InsertVar_(filedata, _line, args);
    return _line;
}

void InsertVar(std::string &line, std::vector<std::string> &args, std::shared_ptr<ATRC_FD> filedata) {
    _W_InsertVar_(filedata.get(), line, args);
}

std::string InsertVar_S(
    const std::string &line,
    std::vector<std::string> &args, 
    std::shared_ptr<ATRC_FD> filedata
) {
    return _W_InsertVar_S_(filedata.get(), line, args);
}