#include "test/TestLexer.h"

#define RUN_TESTS

int main(int argc, char ** args) {
    if (argc <= 1) {
        std::cout
            << "This demo requires a working directory as a command-line argument.\n";

        return 1;
    }

    Tests::WorkingDirectory = std::string(args[1]);

    #ifdef RUN_TESTS
    {
        Tests::Init();
        Tests::Run(std::cout);
    }
    #else
    {
        std::vector<std::string>
        files = {
            "/res/input03_incorrect.txt",
            "/res/input04_incorrect.txt"
        };

        for (auto relFilePath : files) {
            FileReader in;

            if (!FileReader::New(
                Tests::WorkingDirectory + relFilePath,
                in
            ))
                continue;

            auto result = Json::RunMyParser(in.Stream());

            if (!result.Success) {
                std::cout
                    << result.Message
                    << '\n'
                    ;

                continue;
            }

            #define HEYA(a, b) [](auto a) { return b; }

            float expense;

            result.Machine
                ->GetResultSet()
                ["PERSONS"]
                .Where(HEYA(p, p["WHO"].Equals("Janet")))
                .back()
                ["WEEK"]
                .Where(HEYA(p, p["NUMBER"].Equals("5")))
                .back()
                ["EXPENSE"]
                .Where(HEYA(p, p["WHAT"].Equals("Car")))
                .back()
                ["AMOUNT"]
                .AsFloat(expense)
                ;

            std::cout << expense;

            #undef HEYA
        }
    }
    #endif

    return 0;
}
