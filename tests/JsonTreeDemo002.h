#pragma once
#ifndef _JSONTREEDEMO002_H
#define _JSONTREEDEMO002_H

#include "../src/JsonParser.h"
#include "../src/JsonTree.h"
#include "../src/JsonContext.h"

namespace Json {
    class MyTreeFactory: public ITreeFactory<Tree<Pointer>> {
        public:
            virtual ~MyTreeFactory() = default;

            virtual ptr_t NewObject(
                std::vector<std::string> keys,
                std::vector<ptr_t> values
            ) override;

            virtual ptr_t NewList(
                std::vector<ptr_t> values
            ) override;

            virtual ptr_t NewString(
                const std::string &
            ) override;

            virtual ptr_t NewNumeric(
                Homonumeric
            ) override;
    };

    class MyPostorderTreeVisitor: public ITreeVisitor<Pointer> {
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