#pragma once
#ifndef _JSONTREEDEMO002_H
#define _JSONTREEDEMO002_H

#include "../src/JsonTree.h"
#include "../src/JsonContext.h"

namespace Json {
    class MyPostorderTreeVisitor: public ITreeVisitor {
        private:
            Context<> * _machine;
        public:
            MyPostorderTreeVisitor(
                Context<> * machine
            ):  _machine(machine) {}

            virtual ~MyPostorderTreeVisitor() = default;
            virtual Pointer ForString(const std::string &) override;
            virtual Pointer ForNumeric(Homonumeric) override;
            virtual Pointer ForObject(
                const std::vector<std::string> &,
                std::vector<Pointer> &&
            ) override;
            virtual Pointer ForList(std::vector<Pointer> &&) override;
    };
};

#endif