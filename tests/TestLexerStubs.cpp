#include "TestLexer.h"

void Tests::Init() {
    if (!WorkingDirectory.compare(""))
        std::cout
            << "Tests: WARNING: The working directory has not been set.\n";

    _list = {
        {
            "StreamEnumerator_Should_EnumerateStringStream",
            [](std::string & actual, std::string & expected) -> bool {
                std::string inString;
                std::string expectedString;

                inString = "It's all I have to bring today";
                expected =
                    "[I]\n[t]\n[']\n[s]\n[ ]\n[a]\n[l]\n[l]\n[ ]\n[I]\n[ ]\n[h]\n[a]\n[v]\n[e]\n[ ]\n[t]\n[o]\n[ ]\n[b]\n[r]\n[i]\n[n]\n[g]\n[ ]\n[t]\n[o]\n[d]\n[a]\n[y]\n"
                    ;

                std::stringstream ss(inString);
                std::ostringstream testOut;

                std::unique_ptr<IEnumerator> myEnumerator
                    = std::make_unique<StreamEnumerator>(ss);

                ToStream(*myEnumerator, testOut);
                actual = testOut.str();
                return !expected.compare(actual);
            }
        },
        {
            "Lexer_Should_LexBasicLineOfCpp_WithMinimalWhiteSpace",
            [](std::string & actual, std::string & expected) -> bool {
                std::string testString =
                    "for(int _index1=0;_index2<=12;++_index3){";
                std::vector<std::string> expectedTokens = {
                    "Word: [for]",
                    "Punctuation: [(]",
                    "Word: [int]",
                    "WhiteSpace: [ ]",
                    "Word: [_index1]",
                    "Punctuation: [=]",
                    "Number: [0]",
                    "Punctuation: [;]",
                    "Word: [_index2]",
                    "Punctuation: [<]",
                    "Punctuation: [=]",
                    "Number: [12]",
                    "Punctuation: [;]",
                    "Punctuation: [+]",
                    "Punctuation: [+]",
                    "Word: [_index3]",
                    "Punctuation: [)]",
                    "Punctuation: [{]",
                };

                return TestLexer(
                    testString,
                    std::move(expectedTokens),
                    actual,
                    expected,
                    false
                );
            }
        },
        {
            "Lexer_Should_LexBasicLineOfCpp_IgnoringWhiteSpace",
            [](std::string & actual, std::string & expected) -> bool {
                std::string testString =
                    "for (int _index1 = 0; _index2 <= 12; ++_index3) {";
                std::vector<std::string> expectedTokens = {
                    "Word: [for]",
                    "Punctuation: [(]",
                    "Word: [int]",
                    "Word: [_index1]",
                    "Punctuation: [=]",
                    "Number: [0]",
                    "Punctuation: [;]",
                    "Word: [_index2]",
                    "Punctuation: [<]",
                    "Punctuation: [=]",
                    "Number: [12]",
                    "Punctuation: [;]",
                    "Punctuation: [+]",
                    "Punctuation: [+]",
                    "Word: [_index3]",
                    "Punctuation: [)]",
                    "Punctuation: [{]",
                };

                return TestLexer(
                    testString,
                    std::move(expectedTokens),
                    actual,
                    expected,
                    true
                );
            }
        },
        {
            "Lexer_Should_LexBasicLineOfCpp",
            [](std::string & actual, std::string & expected) -> bool {
                std::string testString =
                    "for (int \t_index1 = 0; \n _index2 <= 12; \n\t ++_index3) {";
                std::vector<std::string> expectedTokens = {
                    "Word: [for]",
                    "WhiteSpace: [ ]",
                    "Punctuation: [(]",
                    "Word: [int]",
                    "WhiteSpace: [ \\t]",
                    "Word: [_index1]",
                    "WhiteSpace: [ ]",
                    "Punctuation: [=]",
                    "WhiteSpace: [ ]",
                    "Number: [0]",
                    "Punctuation: [;]",
                    "WhiteSpace: [ \\n ]",
                    "Word: [_index2]",
                    "WhiteSpace: [ ]",
                    "Punctuation: [<]",
                    "Punctuation: [=]",
                    "WhiteSpace: [ ]",
                    "Number: [12]",
                    "Punctuation: [;]",
                    "WhiteSpace: [ \\n\\t ]",
                    "Punctuation: [+]",
                    "Punctuation: [+]",
                    "Word: [_index3]",
                    "Punctuation: [)]",
                    "WhiteSpace: [ ]",
                    "Punctuation: [{]",
                };

                return TestLexer(
                    testString,
                    std::move(expectedTokens),
                    actual,
                    expected,
                    false
                );
            }
        },
        {
            "Lexer_Should_LexStringAssignment",
            [](std::string & actual, std::string & expected) -> bool {
                std::string testString =
                    "__dickinson := \"It's all I have to bring today\";";
                std::vector<std::string> expectedTokens = {
                    "Word: [__dickinson]",
                    "WhiteSpace: [ ]",
                    "Punctuation: [:]",
                    "Punctuation: [=]",
                    "WhiteSpace: [ ]",
                    "String: [It's all I have to bring today]",
                    "Punctuation: [;]",
                };

                return TestLexer(
                    testString,
                    std::move(expectedTokens),
                    actual,
                    expected,
                    false
                );
            }
        },
        {
            "Lexer_Should_LexCCode",
            [](std::string & actual, std::string & expected) -> bool {
                std::string testString =
                    "while (int _number3 = -12.37 && my3_str__4 == \"Everyone has UNO,\\nit came free with your\\tX-box.\");;";
                std::vector<std::string> expectedTokens = {
                    "Word: [while]",
                    "Punctuation: [(]",
                    "Word: [int]",
                    "Word: [_number3]",
                    "Punctuation: [=]",
                    "Punctuation: [-]",
                    "Float: [12.37]",
                    "Punctuation: [&]",
                    "Punctuation: [&]",
                    "Word: [my3_str__4]",
                    "Punctuation: [=]",
                    "Punctuation: [=]",
                    "String: [Everyone has UNO,\nit came free with your\tX-box.]",
                    "Punctuation: [)]",
                    "Punctuation: [;]",
                    "Punctuation: [;]"
                };

                return TestLexerDemo002(
                    testString,
                    std::move(expectedTokens),
                    actual,
                    expected
                );
            }
        },
        {
            "JsonContextToString_Should_MatchJsonFileContent",
            [](std::string & actual, std::string & expected) -> bool {
                bool success = TestJsonParserDemo002(
                    "res/input001.json",
                    "res/expected011.txt",
                    actual,
                    expected
                );

                return success;
            }
        },
        {
            "JsonResultSet_Should_MatchStringInNestedObject",
            [](std::string & actual, std::string & expected) -> bool {
                FileReader inputReader;

                if (!StartFileReader(
                    "res/input001.json",
                    actual,
                    inputReader
                )) {
                    expected = "Input file opened successfully";
                    return false;
                }

                auto machine = Json::GetContext(inputReader.Stream());

                actual =
                    machine->GetResultSet()
                        .At("link")
                        .At("url")
                        .ToString()
                        ;
                expected =
                    "https://json.org/example.html";

                if (expected.compare(actual))
                    return false;

                actual =
                    machine->GetResultSet()
                        .ToString()
                        ;
                expected =
                    "{ \"link\": { \"url\": \"https://json.org/example.html\", \"retrieved\": \"2022_06_19\" }, \"glossary\": { \"title\": \"example glossary\", \"GlossDiv\": { \"title\": \"S\", \"GlossList\": { \"GlossEntry\": { \"ID\": \"SGML\", \"SortAs\": \"SGML\", \"GlossTerm\": \"Standard Generalized Markup Language\", \"Acronym\": \"SGML\", \"Abbrev\": \"ISO 8879:1986\", \"GlossDef\": { \"para\": \"A meta-markup language, used to create markup languages such as DocBook.\", \"GlossSeeAlso\": [ \"GML\", \"XML\" ] }, \"GlossSee\": \"markup\" } } } } }";

                if (expected.compare(actual))
                    return false;

                actual =
                    machine->GetResultSet()
                        .At("glossary")
                        .At("GlossDiv")
                        .At("GlossList")
                        .At("GlossEntry")
                        .At("GlossDef")
                        .At("GlossSeeAlso")
                        .At(1)
                        .ToString()
                        ;
                expected =
                    "XML";

                if (expected.compare(actual))
                    return false;

                actual =
                    machine->GetResultSet()
                        ["glossary"]
                        ["GlossDiv"]
                        ["GlossList"]
                        ["GlossEntry"]
                        ["GlossDef"]
                        ["GlossSeeAlso"]
                        [1]
                        .ToString()
                        ;
                expected =
                    "XML";

                if (expected.compare(actual))
                    return false;

                actual =
                    machine->GetResultSet()
                        .At("glo")
                        .At("Glo")
                        .At("Glo")
                        .At("Glo")
                        .At("Glo")
                        .At("Glo")
                        .At(1)
                        .ToString()
                        ;
                expected =
                    "";

                if (expected.compare(actual))
                    return false;

                actual =
                    machine->GetResultSet()
                        ["glo"]
                        ["Glo"]
                        ["Glo"]
                        ["Glo"]
                        ["Glo"]
                        ["Glo"]
                        [1]
                        .ToString()
                        ;
                expected =
                    "";

                return !expected.compare(actual);
            }
        /*
        // TODO quarantine
        },
        {
            "JsonParser_Should_IdentifyHierarchyInJsonString",
            [](std::string & actual, std::string & expected) -> bool {
                return TestJsonParserDemo001(
                    "res/input001.json",
                    "res/expected001.txt",
                    actual,
                    expected
                );
            }
        */
        }
    };
}
