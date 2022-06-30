#include "JsonMachine.h"

JsonPointer JsonPointerMachine::NewObject() {
    _objects.push_back(std::move(jobject_t()));
    return JsonPointer {
        JsonType::OBJECT,
        _objects.size() - 1
    };
}

JsonPointer JsonPointerMachine::NewList() {
    _lists.push_back(std::move(jlist_t()));
    return JsonPointer {
        JsonType::LIST,
        _list.size() - 1
    };
}

JsonPointer JsonPointerMachine::NewString(const std::string & value) {
    _strings.push_back(value);
    return JsonPointer {
        JsonType::STRING,
        _strings.size() - 1
    };
}

JsonPointer JsonPointerMachine::NewInteger(int value) {
    _integers.push_back(value);
    return JsonPointer {
        JsonType::INTEGER,
        _integers.size() - 1
    };
}

JsonPointer JsonPointerMachine::NewFloat(float value) {
    _floats.push_back(value);
    return JsonPointer {
        JsonType::FLOAT,
        _floats.size() - 1
    };
}

JsonPointer JsonPointerMachine::NewBoolean(bool value) {
    _booleans.push_back(value);
    return JsonPointer {
        JsonType::BOOLEAN,
        _booleans.size() - 1
    };
}
