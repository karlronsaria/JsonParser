#include <unordered_map>
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
    private:
        std::unordered_map<key_t, std::string>
        _strings;

        std::unordered_map<key_t, int>
        _integers;

        std::unordered_map<key_t, float>
        _floats;

        // TODO: find a more efficient solution
        std::unordered_map<key_t, bool>
        _booleans;

        // TODO: determine if this is really necessary
        std::vector<key_t>
        _undeclared;

        std::unordered_map<key_t, jlist_t>
        _lists;

        std::unordered_map<key_t, jobject_t>
        _objects;
    public:
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

class JsonStVisitor {
    // TODO
};

class JsonSt {
    public:
        // TODO
        virtual void Evaluate(JsonStVisitor *) = 0;
}

class String_JsonSt: public JsonSt {
    private:
        std::string _str;
    public:
        const std::string & str();
        String_JsonST(const std::string &);
        virtual ~String_JsonST() {}
        virtual void Evaluate(JsonStVisitor *) override;
}

class Object_JsonSt: public JsonSt {
    public:
}