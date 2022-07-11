#include "MyJson.h"

#ifdef JSON_TREE_PARAMETER
#undef JSON_TREE_PARAMETER
#endif
#define JSON_TREE_PARAMETER Json::Pointer

#ifdef JSON_FACTORY_PTR 
#undef JSON_FACTORY_PTR
#endif
#define JSON_FACTORY_PTR Json::ITreeFactory<Json::Tree<JSON_TREE_PARAMETER>>::ptr_t

JSON_FACTORY_PTR
Json::MyTreeFactory::NewObject(
    std::vector<std::string> keys,
    std::vector<ptr_t> values
) {
    return std::make_unique<Object<JSON_TREE_PARAMETER>>(
        std::move(keys),
        std::move(values)
    );
}

JSON_FACTORY_PTR
Json::MyTreeFactory::NewList(
    std::vector<ptr_t> values
) {
    return std::make_unique<List<JSON_TREE_PARAMETER>>(
        std::move(values)
    );
}

JSON_FACTORY_PTR
Json::MyTreeFactory::NewString(
    const std::string & value
) {
    return std::make_unique<String<JSON_TREE_PARAMETER>>(value);
}

JSON_FACTORY_PTR
Json::MyTreeFactory::NewNumeric(
    Homonumeric value
) {
    return std::make_unique<Numeric<JSON_TREE_PARAMETER>>(value);
}

JSON_TREE_PARAMETER
Json::MyPostorderTreeVisitor::ForObject(
    const std::vector<std::string> & keys,
    std::vector<JSON_TREE_PARAMETER> && values
) {
    auto objectPtr = _machine->NewObject();
    auto & object = _machine->Object(objectPtr.key);

    for (int i = 0; i < keys.size(); ++i)
        // TODO: Add collision checking
        object.add(keys[i], values[i]);

    return objectPtr;
}

JSON_TREE_PARAMETER
Json::MyPostorderTreeVisitor::ForList(
    std::vector<JSON_TREE_PARAMETER> && values
) {
    auto listPtr = _machine->NewList();
    auto & list = _machine->List(listPtr.key);

    for (auto & value : values)
        list.push_back(value);

    return listPtr;
}

JSON_TREE_PARAMETER
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

    return JSON_TREE_PARAMETER();
}

JSON_TREE_PARAMETER
Json::MyPostorderTreeVisitor::ForString(
    const std::string & value
) {
    return _machine->NewString(value);
}

std::shared_ptr<const Json::Context>
Json::GetContext(
    std::istream & inputStream
) {
    auto enumerator = std::make_shared<StreamEnumerator>(inputStream);
    auto lexer = std::make_shared<Lexer>(enumerator);
    auto machine = std::make_shared<Json::Context>();
    auto visitor = std::make_shared<Json::MyPostorderTreeVisitor>(machine);
    auto tree = Json::Parser<Json::Tree<Json::Pointer>>::Tree<Json::MyTreeFactory>(lexer);
    tree->Accept(visitor);
    return machine;
}

#undef JSON_FACTORY_PTR
#undef JSON_TREE_PARAMETER