#include "JsonTreeDemo002.h"

Json::Pointer
Json::MyPostorderTreeVisitor::ForObject(
    const std::vector<std::string> & keys,
    std::vector<Json::Pointer> && values
) {
    auto objectPtr = _machine->NewObject();
    auto & object = _machine->Object(objectPtr.key);

    for (int i = 0; i < keys.size(); ++i)
        // TODO: Add collision checking
        object.add(keys[i], values[i]);

    return objectPtr;
}

Json::Pointer
Json::MyPostorderTreeVisitor::ForList(
    std::vector<Json::Pointer> && values
) {
    auto listPtr = _machine->NewList();
    auto & list = _machine->List(listPtr.key);

    for (auto & value : values)
        list.push_back(value);

    return listPtr;
}

Json::Pointer
Json::MyPostorderTreeVisitor::ForNumeric(
    Homonumeric value
) {
    switch (value.Mode) {
        case Homonumeric::Mode::BOOLEAN:
            return _machine->NewBoolean(value.Payload.Boolean);
        case Homonumeric::Mode::INTEGER:
            return _machine->NewInteger(value.Payload.Integer);
        case Homonumeric::Mode::FLOAT:
            return _machine->NewFloat(value.Payload.Float);
        default:
            // TODO: Consider adding exception-handling here
            break;
    }

    return Json::Pointer { Json::Type::NIL, 0 };
}

Json::Pointer
Json::MyPostorderTreeVisitor::ForString(
    const std::string & value
) {
    return _machine->NewString(value);
}
