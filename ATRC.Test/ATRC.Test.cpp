#include "ATRC.Test.h"
#include <iostream>
#include "../Wrappers/C++/include/ATRC.hpp"

int cpp_main(void)
{
    return 0;
    std::cout << "ATRC Test Application\n" << std::endl;

    atrc::_ATRC_FD fd = atrc::_ATRC_FD("./ATRC.Test/test.atrc", ATRC_READ_ONLY);
    fd._SetAutoSave(true);
    std::cout << fd["test"] << std::endl;

    fd._WriteCommentToBottom("This is aaa comment at the bottom.");
    return 0;
}