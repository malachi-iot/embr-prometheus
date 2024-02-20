#include <iostream>

int old_main()
{
    std::cout << "Hello, World!" << std::endl;
    return 0;
}

namespace embr::prometheus {

void synthetic()
{
    std::cout << "Hello" << std::endl;
}

}
