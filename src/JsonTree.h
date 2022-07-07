#pragma once
#ifndef _JSONTREE_H
#define _JSONTREE_H

#include "Homonumeric.h"
#include <memory>
#include <string>
#include <vector>

#include "JsonContext.h"

namespace Json {
    class Tree;

    // Postorder-traversal visitor
    class ITreeVisitor {
        public:
            virtual ~ITreeVisitor() = default;
            virtual Pointer ForString(const std::string &) = 0;
            virtual Pointer ForNumeric(Homonumeric) = 0;
            virtual Pointer ForObject(
                const std::vector<std::string> &,
                std::vector<Pointer> &&
            ) = 0;
            virtual Pointer ForList(std::vector<Pointer> &&) = 0;
    };

    class Tree {
        public:
            virtual ~Tree() = default;
            virtual Pointer Accept(ITreeVisitor *) = 0;
    };

    typedef std::unique_ptr<Tree> tree_t;

    class Object: public Tree {
        private:
            std::vector<std::string> _keys;
            std::vector<tree_t> _values;
        public:
            virtual ~Object() = default;

            Object(
                std::vector<std::string> keys,
                std::vector<tree_t> values
            ):  _keys(std::move(keys)),
                _values(std::move(values)) {}

            virtual Pointer Accept(ITreeVisitor *) override;
    };

    class List: public Tree {
        private:
            std::vector<tree_t> _values;
        public:
            virtual ~List() = default;

            List(std::vector<tree_t> values):
                _values(std::move(values)) {}

            virtual Pointer Accept(ITreeVisitor *) override;
    };

    class String: public Tree {
        private:
            std::string _payload;
        public:
            virtual ~String() = default;

            String(const std::string & payload):
                _payload(payload) {}

            virtual Pointer Accept(ITreeVisitor *) override;
    };

    class Numeric: public Tree {
        private:
            Homonumeric _payload;
        public:
            virtual ~Numeric() = default;

            Numeric(Homonumeric payload):
                _payload(payload) {}

            virtual Pointer Accept(ITreeVisitor *) override;
    };
};

#endif