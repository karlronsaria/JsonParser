#include <unordered_map>
#include <string>
#include <vector>

typedef int key_t;

enum JsonType {
    STRING,
    INTEGER,
    FLOAT,
    BOOLEAN,
    OBJECT,
    LIST,
    NIL
};

struct JsonPointer {
    JsonType type;
    key_t key;
};

typedef std::vector<JsonPointer>
jlist_t;

typedef std::unordered_map<std::string, JsonPointer>
jobject_t;

class IJsonPointerMachineVisitor {
    public:
        virtual bool ForString(const std::string &) = 0;
        virtual bool ForInteger(int) = 0;
        virtual bool ForFloat(float) = 0;
        virtual bool ForBoolean(bool) = 0;
        virtual bool ForList(jlist_t) = 0;
        virtual bool ForObject(jobject_t) = 0;
};

class JsonPointerMachine {
    // private:
    public:
        std::vector<std::string>
        _strings;

        std::vector<int>
        _integers;

        std::vector<float>
        _floats;

        // TODO: find a more efficient solution
        std::vector<bool>
        _booleans;

        // // TODO: determine if this is really necessary
        // std::vector<key_t>
        // _undeclared;

        std::vector<jlist_t>
        _lists;

        std::vector<jobject_t>
        _objects;
    public:
        JsonPointer NewObject();
        JsonPointer NewList();
        JsonPointer NewString(const std::string &);
        JsonPointer NewInteger(int);
        JsonPointer NewFloat(float);
        JsonPointer NewBoolean(bool);

        jobject_t & Object(int);
        jlist_t & List(int);
        std::string & String(int);
        int & Integer(int);
        float & Float(int);
        bool & Boolean(int);

        bool Accept(
                IJsonPointerMachineVisitor * visitor,
                JsonPointer pointer
            )
        {
            switch (pointer.type) {
                case JsonType::STRING:
                    return visitor->ForString();
                case JsonType::INTEGER:
                    return visitor->ForInteger();
                case JsonType::FLOAT:
                    return visitor->ForFloat();
                case JsonType::BOOLEAN:
                    return visitor->ForBoolean();
                case JsonType::OBJECT:
                    return visitor->ForObject();
                case JsonType::LIST:
                    return visitor->ForList();
                case JsonType::NIL:
                    break;
            }
        }
};