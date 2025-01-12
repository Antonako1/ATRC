#include "ATRC.Test.h"
#include <iostream>
#include <ATRC.h>
using namespace atrc;
void cpp_main(void)
{
	ATRC_FD fd = ATRC_FD("file.atrc");
    if (!fd.CheckStatus()) {
		std::cout << "File parsed unsuccesfully!" << std::endl;
		return;
	}
    std::cout << fd["var_name"] << std::endl;

    C_PATRC_FD c_fd = fd.ToCStruct();
    std::string line = fd["block_name"]["key"];
    const char* args[] = {"Hello everyone", nullptr};
    std::cout << "Before: " << line << std::endl;
    char* res = InsertVar_S(line.c_str(), args);
    std::cout << "After: " << res << std::endl;
    delete[] res;
}