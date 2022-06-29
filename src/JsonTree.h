#pragma once
#ifndef _JSONTREE_H
#define _JSONTREE_H

#include "Homonumeric.h"
#include <memory>
#include <string>
#include <vector>

class JsonTree;

class IJsonTreeVisitor {
    public:
        virtual ~IJsonTreeVisitor() = default;
        virtual void ForString(const std::string & payload) = 0;
        virtual void ForNumeric(Homonumeric payload) = 0;

        virtual void StartObject() = 0;
        virtual void EndObject() = 0;

        virtual void StartList() = 0;
        virtual void EndList() = 0;

        virtual void StartPair(const std::string & key) = 0;
        virtual void EndPair() = 0;

        virtual void StartValue() = 0;
        virtual void EndValue() = 0;
};

class JsonTree {
    public:
        virtual ~JsonTree() = default;
        virtual void Accept(IJsonTreeVisitor *) = 0;
};

typedef std::unique_ptr<JsonTree> jsonptr_t;

class JsonPair: public JsonTree {
    private:
        std::string _key;
        jsonptr_t _value;
    public:
        virtual ~JsonPair() = default;
        JsonPair(const std::string & key, jsonptr_t value):
            _key(key),
            _value(std::move(value)) {}
        virtual void Accept(IJsonTreeVisitor *) override;
};

class JsonObject: public JsonTree {
    private:
        std::vector<jsonptr_t> _pairs;
    public:
        virtual ~JsonObject() = default;
        JsonObject(std::vector<jsonptr_t> pairs):
            _pairs(std::move(pairs)) {}
        virtual void Accept(IJsonTreeVisitor *) override;
};

class JsonList: public JsonTree {
    private:
        std::vector<jsonptr_t> _values;
    public:
        virtual ~JsonList() = default;
        JsonList(std::vector<jsonptr_t> values):
            _values(std::move(values)) {}
        virtual void Accept(IJsonTreeVisitor *) override;
};

class JsonString: public JsonTree {
    private:
        std::string _payload;
    public:
        virtual ~JsonString() = default;
        JsonString(const std::string & payload):
            _payload(payload) {}
        virtual void Accept(IJsonTreeVisitor *) override;
};

class JsonNumeric: public JsonTree {
    private:
        Homonumeric _payload;
    public:
        virtual ~JsonNumeric() = default;
        JsonNumeric(Homonumeric payload):
            _payload(payload) {}
        virtual void Accept(IJsonTreeVisitor *) override;
};

#endif