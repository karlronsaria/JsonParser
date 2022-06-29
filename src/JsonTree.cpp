#include "JsonTree.h"

void JsonPair::Accept(IJsonTreeVisitor * visitor) {
    visitor->StartPair(_key);
    visitor->StartValue();
    _value->Accept(visitor);
    visitor->EndValue();
    visitor->EndPair();
}

void JsonObject::Accept(IJsonTreeVisitor * visitor) {
    visitor->StartObject();

    for (auto & pair : _pairs)
        pair->Accept(visitor);

    visitor->EndObject();
}

void JsonList::Accept(IJsonTreeVisitor * visitor) {
    visitor->StartList();

    for (auto & value : _values) {
        visitor->StartValue();
        value->Accept(visitor);
        visitor->EndValue();
    }

    visitor->EndList();
}

void JsonString::Accept(IJsonTreeVisitor * visitor) {
    visitor->ForString(_payload);
}

void JsonNumeric::Accept(IJsonTreeVisitor * visitor) {
    visitor->ForNumeric(_payload);
}