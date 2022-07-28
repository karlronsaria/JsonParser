#include "tests/TestLexer.h"

#define RUN_TESTS

int main(int argc, char ** args) {
    if (argc <= 1) {
        std::cout
            << "This demo requires a working directory as a command-line argument.\n";

        return 1;
    }

#ifndef RUN_TESTS
    Tests::WorkingDirectory = std::string(args[1]);
    FileReader in;

    if (!FileReader::New(Tests::WorkingDirectory + "/res/input02.json", in))
        return 1;

    auto machine = Json::GetMachine(in.Stream());
    float expense;

    machine
        ->GetResultSet()
        ["PERSONS"]
        .Where([](auto p) { return p["WHO"].Equals("Janet"); })
        .back()
        ["WEEK"]
        .Where([](auto p) { return p["NUMBER"].Equals("5"); })
        .back()
        ["EXPENSE"]
        .Where([](auto p) { return p["WHAT"].Equals("Car"); })
        .back()
        ["AMOUNT"]
        .AsFloat(expense)
        ;

    std::cout
        << expense;
#else
    Tests::WorkingDirectory = std::string(args[1]);
    Tests::Init();
    Tests::Run(std::cout);
#endif
    return 0;
}
