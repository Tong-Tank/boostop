#include "test.hpp"

int main(int argc, char* argv[])
{

    auto t = new CreateSMIOptions(2);
    t->parseCmdLineArgs(argc, argv);

    delete t;
    return 0;
}