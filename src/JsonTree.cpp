#include "JsonTree.h"

Json::Pointer Json::Object::Accept(Json::ITreeVisitor * visitor) {
    std::vector<Json::Pointer> postvalues;

    for (auto & value : _values)
        postvalues.push_back(std::move(
            value->Accept(visitor)
        ));

    return visitor->ForObject(
        _keys,
        std::move(postvalues)
    );
}

Json::Pointer Json::List::Accept(Json::ITreeVisitor * visitor) {
    std::vector<Json::Pointer> postvalues;

    for (auto & value : _values)
        postvalues.push_back(std::move(value->Accept(visitor)));

    return visitor->ForList(std::move(postvalues));
}

Json::Pointer Json::String::Accept(Json::ITreeVisitor * visitor) {
    return visitor->ForString(_payload);
}

Json::Pointer Json::Numeric::Accept(Json::ITreeVisitor * visitor) {
    return visitor->ForNumeric(_payload);
}