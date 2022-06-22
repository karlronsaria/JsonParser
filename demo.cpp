#include "tests/TestLexer.h"

int main(int argc, char ** args) {
    if (argc <= 1) {
        std::cout
            << "This demo requires a working directory as a command-line argument.\n";

        return 1;
    }

    Tests::WorkingDirectory = std::string(args[1]);
    Tests::Init();
    Tests::Run(std::cout);
    return 0;
}