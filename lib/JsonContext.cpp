#include "JsonContext.h"

std::string
Json::ToString(const Json::Type & typeCode) {
    switch (typeCode) {
        case Json::Type::STRING:
            return "String";
        case Json::Type::INTEGER:
            return "Integer";
        case Json::Type::FLOAT:
            return "Float";
        case Json::Type::BOOLEAN:
            return "Boolean";
        case Json::Type::OBJECT:
            return "Object";
        case Json::Type::LIST:
            return "List";
        case Json::Type::NIL:
            return "Nil";
    }

    return "";
}
