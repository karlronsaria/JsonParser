#pragma once
#ifndef _JSONMACHINE_H
#define _JSONMACHINE_H

#include <bitset>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

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

    template <size_t MAX_BOOLEANS = 1024ULL>
    class Context {
        public:
            class ResultSet {
                friend class Context;

                private:
                    Context const * const
                    _context;

                    Pointer
                    _pointer;

                    std::string RecurseToString(Pointer) const;
                    ResultSet(Context const * const, const Pointer &);
                public:
                    ResultSet();
                    ResultSet(const ResultSet &) = default;
                    virtual ~ResultSet() = default;
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

            struct {
                std::bitset<MAX_BOOLEANS> bits;
                size_t size = 0;
            }
            _booleans;

            std::vector<list_t>
            _lists;

            std::vector<object_t>
            _objects;
        public:
            virtual ~Context() = default;

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

template <size_t M>
Json::Pointer
Json::Context<M>::NewObject() {
    _objects.push_back(std::move(Json::object_t()));
    return Json::Pointer {
        Json::Type::OBJECT,
        (int)_objects.size() - 1
    };
}

template <size_t M>
Json::Pointer
Json::Context<M>::NewList() {
    _lists.push_back(std::move(Json::list_t()));
    return Json::Pointer {
        Json::Type::LIST,
        (int)_lists.size() - 1
    };
}

template <size_t M>
Json::Pointer
Json::Context<M>::NewString(const std::string & value) {
    _strings.push_back(value);
    return Json::Pointer {
        Json::Type::STRING,
        (int)_strings.size() - 1
    };
}

template <size_t M>
Json::Pointer
Json::Context<M>::NewInteger(int value) {
    _integers.push_back(value);
    return Json::Pointer {
        Json::Type::INTEGER,
        (int)_integers.size() - 1
    };
}

template <size_t M>
Json::Pointer
Json::Context<M>::NewFloat(float value) {
    _floats.push_back(value);
    return Json::Pointer {
        Json::Type::FLOAT,
        (int)_floats.size() - 1
    };
}

template <size_t M>
Json::Pointer
Json::Context<M>::NewBoolean(bool value) {
    _booleans.bits.set(_booleans.size, value);
    _booleans.size++;
    return Json::Pointer {
        Json::Type::BOOLEAN,
        (int)_booleans.size - 1
    };
}

#define DEFINE_JSONCONTEXT_GETTER(RETURN, NAME, RESOURCE) \
    template <size_t M> \
    RETURN & \
    Json::Context<M>::NAME(key_t key) { \
        return RESOURCE[key]; \
    }

DEFINE_JSONCONTEXT_GETTER(Json::object_t, Object, _objects)
DEFINE_JSONCONTEXT_GETTER(Json::list_t, List, _lists)
DEFINE_JSONCONTEXT_GETTER(std::string, String, _strings)
DEFINE_JSONCONTEXT_GETTER(int, Integer, _integers)
DEFINE_JSONCONTEXT_GETTER(float, Float, _floats)
#undef DEFINE_JSONCONTEXT_GETTER

template <size_t M>
void
Json::Context<M>::SetBoolean(key_t key, bool value) {
    _booleans.bits.set((size_t)key, value);
}

#define DEFINE_JSONCONTEXT_CONST_GETTER(RETURN, NAME, RESOURCE) \
    template <size_t M> \
    const RETURN & \
    Json::Context<M>::NAME(key_t key) const { \
        return RESOURCE.at(key); \
    }

DEFINE_JSONCONTEXT_CONST_GETTER(Json::object_t, Object, _objects)
DEFINE_JSONCONTEXT_CONST_GETTER(Json::list_t, List, _lists)
DEFINE_JSONCONTEXT_CONST_GETTER(std::string, String, _strings)
DEFINE_JSONCONTEXT_CONST_GETTER(int, Integer, _integers)
DEFINE_JSONCONTEXT_CONST_GETTER(float, Float, _floats)
#undef DEFINE_JSONCONTEXT_GETTER

template <size_t M>
bool
Json::Context<M>::Boolean(key_t key) const {
    return _booleans.bits[(size_t)key];
}

template <size_t M>
std::string
Json::Context<M>::ToString() const {
    std::ostringstream oss;

    oss << "Objects:\n";

    for (int i = 0; i < _objects.size(); ++i) {
        oss << "  Pair " << i << ":\n";
        auto & object = _objects[i];

        for (int j = 0; j < object.keys().size(); ++j) {
            auto & key = object.keys().at(j);
            auto value = object.at(key);

            oss << "    " << j << ":\n"
                << "      Key: [" << key << "]\n"
                << "      Value:\n"
                << "        Type: " << Json::ToString(value.type) << '\n'
                << "        Index: " << value.key << '\n';
        }
    }

    oss << "Lists:\n";

    for (int i = 0; i < _lists.size(); ++i) {
        oss << "  " << i << ":\n";
        auto & list = _lists[i];

        for (auto & value : list) {
            oss << "    Type: " << Json::ToString(value.type) << '\n'
                << "    Index: " << value.key << '\n';
        }
    }

    oss << "Strings:\n";

    for (int i = 0; i < _strings.size(); ++i)
        oss << "  " << i << ": [" << _strings[i] << "]\n";

    oss << "Integers:\n";

    for (int i = 0; i < _integers.size(); ++i)
        oss << "  " << i << ": [" << _integers[i] << "]\n";

    oss << "Floats:\n";

    for (int i = 0; i < _floats.size(); ++i)
        oss << "  " << i << ": [" << _floats[i] << "]\n";

    oss << "Booleans:\n";

    for (int i = 0; i < _booleans.size; ++i)
        oss << "  " << i << ": [" << _booleans.bits[i] << "]\n";

    return oss.str();
}

template <size_t M>
const typename Json::Context<M>::ResultSet
Json::Context<M>::GetResultSet() const {
    if (_objects.empty())
        return ResultSet(this, Pointer{ Type::NIL, 0 });

    return ResultSet(this, Pointer{ Type::OBJECT, (int)_objects.size() - 1 });
}

template <size_t M>
Json::Context<M>::ResultSet::ResultSet(
    Json::Context<M> const * const context,
    const Json::Pointer & pointer
):  _context(context),
    _pointer(pointer) {}

template <size_t M>
Json::Context<M>::ResultSet::ResultSet():
    _context(nullptr),
    _pointer(Json::Pointer{ Json::Type::NIL, 0 }) {}

#define DEFINE_JSONCONTEXT_RESULTSET_GETTER(NAME, TYPE, TYPE_SYMBOL) \
    template <size_t M> \
    bool \
    Json::Context<M>::ResultSet::As##NAME(TYPE & value) const { \
        if (_pointer.type != TYPE_SYMBOL) \
            return false; \
    \
        value = _context->NAME(_pointer.key); \
        return true; \
    }

DEFINE_JSONCONTEXT_RESULTSET_GETTER(Integer, int, Type::INTEGER)
DEFINE_JSONCONTEXT_RESULTSET_GETTER(Float, float, Type::FLOAT)
DEFINE_JSONCONTEXT_RESULTSET_GETTER(String, std::string, Type::STRING)
#undef DEFINE_JSONCONTEXT_RESULTSET_GETTER

template <size_t M>
typename Json::Context<M>::ResultSet
Json::Context<M>::ResultSet::At(int index) const {
    if (_pointer.type != Type::LIST)
        return ResultSet(_context, Pointer{ Type::NIL, 0 });

    return ResultSet(_context, _context->List(_pointer.key).at(index));
}

template <size_t M>
typename Json::Context<M>::ResultSet
Json::Context<M>::ResultSet::At(const std::string & key) const {
    if (_pointer.type != Type::OBJECT)
        return ResultSet(_context, Pointer{ Type::NIL, 0 });

    return ResultSet(_context, _context->Object(_pointer.key).at(key));
}

template <size_t M>
typename Json::Context<M>::ResultSet
Json::Context<M>::ResultSet::operator[](int index) const {
    return At(index);
}

template <size_t M>
typename Json::Context<M>::ResultSet
Json::Context<M>::ResultSet::operator[](const std::string & key) const {
    return At(key);
}

template <size_t M>
std::string
Json::Context<M>::ResultSet::RecurseToString(
    Json::Pointer value
) const {
    std::ostringstream outss;

    switch (value.type) {
        case Type::STRING:
            outss
                << '"'
                << _context->String(value.key)
                << '"';

            break;
        default:
            outss
                << ResultSet(_context, value).ToString();

            break;
    }

    return outss.str();
}

template <size_t M>
Json::Type
Json::Context<M>::ResultSet::TypeCode() const {
    return _pointer.type;
}

template <size_t M>
bool
Json::Context<M>::ResultSet::IsNil() const {
    return _pointer.type == Type::NIL;
}

template <size_t M>
std::string
Json::Context<M>::ResultSet::ToString() const {
    std::ostringstream outss;

    switch (_pointer.type) {
        case Json::Type::STRING:
            return _context->String(_pointer.key);
        case Json::Type::INTEGER:
            outss << _context->Integer(_pointer.key);
            break;
        case Json::Type::FLOAT:
            outss << _context->Float(_pointer.key);
            break;
        case Json::Type::BOOLEAN:
            outss << _context->Boolean(_pointer.key);
            break;
        case Json::Type::OBJECT:
            {
                outss << "{ ";
                auto object = _context->Object(_pointer.key);
                std::string key;

                for (int i = 0; i < object.keys().size(); ++i) {
                    key = object.keys()[i];

                    outss
                        << '"'
                        << key
                        << "\": "
                        << RecurseToString(object.at(key));

                    if (i < object.keys().size() - 1)
                        outss << ", ";
                }

                outss << " }";
            }

            break;
        case Json::Type::LIST:
            {
                outss << "[ ";
                auto list = _context->List(_pointer.key);

                for (int i = 0; i < list.size(); ++i) {
                    outss << RecurseToString(list.at(i));

                    if (i < list.size() - 1)
                        outss << ", ";
                }

                outss << " ]";
            }

            break;
        case Json::Type::NIL:
            return "";
    }

    return outss.str();
}

#endif
