#include "tests/TestLexer.h"

int main(int argc, char ** args) {
    if (argc <= 1) {
        std::cout
            << "This demo requires a working directory as a command-line argument.\n";

        return 1;
    }

    // std::cout << "Hello, world!\n";

    Tests::WorkingDirectory = std::string(args[1]);
    Tests::Init();
    Tests::Run(std::cout);

/*
    Tests::WorkingDirectory = std::string(args[1]);
    FileReader input;

    if (!FileReader::New(Tests::WorkingDirectory + "/" + "./res/input001.json", input))
        return 1;

    auto context = Json::GetContext(input.Stream());
    auto resultSet = context->GetResultSet();
    std::cout << resultSet.ToString();
*/

    return 0;
}
