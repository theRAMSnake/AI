#include "tasks/manager.hpp"

int main(int argc, char** argv)
{
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <dirname>\n";
        return EXIT_FAILURE;
    }

    tlib::TaskManager mngr;
    mngr.dumpDemoPictures(argv[1]);
    return 0;
}
