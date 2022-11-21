#include "../pop.hpp"
#include <iostream>

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        std::cout << "Usage: pop_reader <pop_filename>" << std::endl;
        return 1;
    }

    std::cout << "Loading pop..." << std::endl;
    auto pop = sori::loadPop(argv[1]);
    std::cout << "Done!" << std::endl;
    pop.print();

    return 0;
}
