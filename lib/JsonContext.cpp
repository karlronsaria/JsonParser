#include "JsonContext.h"

std::string
Json::ToString(const Json::Type & typeCode) {
    switch (typeCode) {
        case Json::Type::STRING:
            return "String";
        case Json::Type::INTEGER:
            return "Integer";
        case Json::Type::FLOAT:
            return "Float";
        case Json::Type::BOOLEAN:
            return "Boolean";
        case Json::Type::OBJECT:
            return "Object";
        case Json::Type::LIST:
            return "List";
        case Json::Type::NIL:
            return "Nil";
    }

    return "";
}

Json::Pointer
Json::Context::NewObject() {
    _objects.push_back(std::move(Json::object_t()));
    return Json::Pointer {
        Json::Type::OBJECT,
        (int)_objects.size() - 1
    };
}

Json::Pointer
Json::Context::NewList() {
    _lists.push_back(std::move(Json::list_t()));
    return Json::Pointer {
        Json::Type::LIST,
        (int)_lists.size() - 1
    };
}

#define DEFINE_JSONCONTEXT_BUILDER(NAME, PARAM_TYPE, VECTOR, TYPE_SYMBOL) \
    Json::Pointer \
    Json::Context::New##NAME(PARAM_TYPE value) { \
        VECTOR.push_back(value); \
        return Json::Pointer { \
            TYPE_SYMBOL, \
            (int)VECTOR.size() - 1 \
        }; \
    }

DEFINE_JSONCONTEXT_BUILDER(String, const std::string &, _strings, Json::Type::STRING)
DEFINE_JSONCONTEXT_BUILDER(Integer, int, _integers, Json::Type::INTEGER)
DEFINE_JSONCONTEXT_BUILDER(Float, float, _floats, Json::Type::FLOAT)
DEFINE_JSONCONTEXT_BUILDER(Boolean, bool, _booleans, Json::Type::BOOLEAN)
#undef DEFINE_JSONCONTEXT_BUILDER

#define DEFINE_JSONCONTEXT_GETTER(RETURN, NAME, RESOURCE) \
    RETURN & \
    Json::Context::NAME(key_t key) { \
        return RESOURCE[key]; \
    }

DEFINE_JSONCONTEXT_GETTER(Json::object_t, Object, _objects)
DEFINE_JSONCONTEXT_GETTER(Json::list_t, List, _lists)
DEFINE_JSONCONTEXT_GETTER(std::string, String, _strings)
DEFINE_JSONCONTEXT_GETTER(int, Integer, _integers)
DEFINE_JSONCONTEXT_GETTER(float, Float, _floats)
#undef DEFINE_JSONCONTEXT_GETTER

void
Json::Context::SetBoolean(key_t key, bool value) {
    _booleans.set((size_t)key, value);
}

#define DEFINE_JSONCONTEXT_CONST_GETTER(RETURN, NAME, RESOURCE) \
    const RETURN & \
    Json::Context::NAME(key_t key) const { \
        return RESOURCE.at(key); \
    }

DEFINE_JSONCONTEXT_CONST_GETTER(Json::object_t, Object, _objects)
DEFINE_JSONCONTEXT_CONST_GETTER(Json::list_t, List, _lists)
DEFINE_JSONCONTEXT_CONST_GETTER(std::string, String, _strings)
DEFINE_JSONCONTEXT_CONST_GETTER(int, Integer, _integers)
DEFINE_JSONCONTEXT_CONST_GETTER(float, Float, _floats)
#undef DEFINE_JSONCONTEXT_GETTER

bool
Json::Context::Boolean(key_t key) const {
    return _booleans[(size_t)key];
}

std::string
Json::Context::ToString() const {
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

    for (int i = 0; i < _booleans.size(); ++i)
        oss << "  " << i << ": [" << _booleans[i] << "]\n";

    return oss.str();
}

const typename Json::Context::ResultSet
Json::Context::GetResultSet() const {
    ResultSet result;
    
    if (_objects.empty())
        result = ResultSet(this, Pointer{ Type::NIL, 0 });

    result = ResultSet(this, Pointer{ Type::OBJECT, (int)_objects.size() - 1 });
    return std::move(result);
}

Json::Context::ResultSet::ResultSet(
    Json::Context::context_ptr_t context,
    const Json::Pointer & pointer
):  _context(context),
    _pointer(pointer) {}

Json::Context::ResultSet::ResultSet():
    _context(nullptr),
    _pointer(Json::Pointer{ Json::Type::NIL, 0 }) {}

#define DEFINE_JSONCONTEXT_RESULTSET_GETTER(NAME, TYPE, TYPE_SYMBOL) \
    bool \
    Json::Context::ResultSet::As##NAME(TYPE & value) const { \
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

typename Json::Context::ResultSet
Json::Context::ResultSet::At(int index) const {
    if (_pointer.type != Type::LIST)
        return ResultSet(_context, Pointer{ Type::NIL, 0 });

    return ResultSet(_context, _context->List(_pointer.key).at(index));
}

typename Json::Context::ResultSet
Json::Context::ResultSet::At(const std::string & key) const {
    if (_pointer.type != Type::OBJECT)
        return ResultSet(_context, Pointer{ Type::NIL, 0 });

    return ResultSet(_context, _context->Object(_pointer.key).at(key));
}

typename Json::Context::ResultSet
Json::Context::ResultSet::operator[](int index) const {
    return At(index);
}

typename Json::Context::ResultSet
Json::Context::ResultSet::operator[](const std::string & key) const {
    return At(key);
}

std::string
Json::Context::ResultSet::RecurseToString(
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

Json::Type
Json::Context::ResultSet::TypeCode() const {
    return _pointer.type;
}

bool
Json::Context::ResultSet::IsNil() const {
    return _pointer.type == Type::NIL;
}

std::string
Json::Context::ResultSet::ToString() const {
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
