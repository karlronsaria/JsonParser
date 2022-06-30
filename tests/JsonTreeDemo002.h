#pragma once
#ifndef _JSONTREEDEMO002_H
#define _JSONTREEDEMO002_H

#include "../src/JsonTree.h"
#include "../src/JsonMachine.h"
#include <stack>

class MyJsonMachineTreeVisitor: public IJsonTreeVisitor {
    private:
        JsonPointerMachine * _machine;
        std::stack<JsonPointer> _objects;
    public:
        MyJsonMachineTreeVisitor();

        virtual void StartObject() override;
        virtual void EndObject() override;

        virtual void StartList() override;
        virtual void EndList() override;

        virtual void StartPair(const std::string &) override;
        virtual void EndPair() override;

        virtual void StartValue() override;
        virtual void EndValue() override;

        virtual void ForString(const std::string &) override;
        virtual void ForNumeric(Homonumeric) override;
};

#endif