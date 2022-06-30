#include "tests/TestLexer.h"

#include "src/JsonMachine.h"

class JsonObject {
    private:
        JsonPointer _ptr;
        JsonPointerMachine * _machine;
    public:
        virtual ~JsonObject() = default;
        bool AsString(std::string &) const;
        bool AsBoolean(bool &) const;
        bool AsInteger(int &) const;
        bool AsFloat(float &) const;
        bool AsList(std::vector<JsonObject> &) const;
        bool GetValue(const std::string &, JsonObject &) const;
        const JsonObject & operator[](const std::string &) const;
        JsonType Type() const;
};

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
    return 0;
}
