#pragma once
#ifndef _MYJSON_H
#define _MYJSON_H

#include "../lib/StreamEnumerator.h"
#include "../lib/Lexer.h"
#include "../lib/JsonParser.h"
#include "../lib/JsonTree.h"
#include "../lib/JsonContext.h"

namespace Json {
    class MyTreeFactory: public ITreeFactory<Tree<Pointer>> {
        public:
            virtual ~MyTreeFactory() = default;

            virtual ptr_t NewObject(
                std::vector<std::string> keys,
                std::vector<ptr_t> values
            ) override;

            virtual ptr_t NewList(std::vector<ptr_t>) override;
            virtual ptr_t NewString(const std::string &) override;
            virtual ptr_t NewNumeric(Homonumeric) override;
    };

    class MyPostorderTreeVisitor: public ITreeVisitor<Pointer> {
        private:
            std::shared_ptr<Context<>> _machine;
        public:
            MyPostorderTreeVisitor(
                std::shared_ptr<Context<>> machine
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

    std::shared_ptr<const Context<>>
    GetContext(std::istream & inputStream);
};

#endif