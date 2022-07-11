#pragma once
#ifndef _JSONMACHINE_H
#define _JSONMACHINE_H

#include "VectorBitset.h"
#include <memory>
#include <sstream>
#include <unordered_map>

namespace Json {
    enum Type {
        STRING,
        INTEGER,
        FLOAT,
        BOOLEAN,
        OBJECT,
        LIST,
        NIL
    };

    std::string ToString(const Type &);

    typedef int key_t;

    struct Pointer {
        Type type = Type::NIL;
        key_t key = 0;
    };

    typedef std::vector<Pointer>
    list_t;

    template <typename T>
    class ObjectDefinition {
        private:
            std::unordered_map<T, Pointer>
            _map;

            std::vector<T>
            _keys;
        public:
            virtual ~ObjectDefinition() = default;

            size_t size() const {
                return _map.size();
            }

            bool hasKey(const T & key) const {
                return _map.find(key) != _map.end();
            }

            bool add(const T & key, Pointer value) {
                if (hasKey(key))
                    return false;

                _map[key] = value;
                _keys.push_back(key);
                return true;
            }

            Pointer at(const T & key) const {
                if (hasKey(key))
                    return _map.at(key);

                return Pointer();
            }

            Pointer operator[](const T & key) const {
                return at(key);
            }

            Pointer & operator[](const T & key) {
                return _map[key];
            }

            const std::vector<std::string> & keys() const {
                return _keys;
            }
    };

    typedef ObjectDefinition<std::string>
    object_t;

    class Machine {
        private:
            typedef
            Machine const *
            machine_ptr_t;
        public:
            class ResultSet {
                friend class Machine;

                private:
                    machine_ptr_t
                    _machine;

                    Pointer
                    _pointer;

                    std::string RecurseToString(Pointer) const;
                    ResultSet(machine_ptr_t, const Pointer &);
                public:
                    ResultSet();
                    ResultSet(const ResultSet &) = default;
                    virtual ~ResultSet() = default;
                    ResultSet & operator=(const ResultSet &) = default;
                    bool AsInteger(int &) const;
                    bool AsFloat(float &) const;
                    bool AsString(std::string &) const;
                    ResultSet At(int) const;
                    ResultSet At(const std::string &) const;
                    ResultSet operator[](int) const;
                    ResultSet operator[](const std::string &) const;
                    Json::Type TypeCode() const;
                    bool IsNil() const;
                    std::string ToString() const;
            };
        private:
            std::vector<std::string>
            _strings;

            std::vector<int>
            _integers;

            std::vector<float>
            _floats;

            vector_bitset<>
            _booleans;

            std::vector<list_t>
            _lists;

            std::vector<object_t>
            _objects;
        public:
            virtual ~Machine() = default;

            Pointer NewObject();
            Pointer NewList();
            Pointer NewString(const std::string &);
            Pointer NewInteger(int);
            Pointer NewFloat(float);
            Pointer NewBoolean(bool);

            object_t & Object(key_t);
            list_t & List(key_t);
            std::string & String(key_t);
            int & Integer(key_t);
            float & Float(key_t);
            void SetBoolean(key_t, bool);

            const object_t & Object(key_t) const;
            const list_t & List(key_t) const;
            const std::string & String(key_t) const;
            const int & Integer(key_t) const;
            const float & Float(key_t) const;
            bool Boolean(key_t) const;

            std::string ToString() const;

            const ResultSet GetResultSet() const;
    };
};

#endif
