#include "JsonMachine.h"

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
Json::Machine::NewObject() {
    _objects.push_back(std::move(Json::object_t()));
    return Json::Pointer {
        Json::Type::OBJECT,
        (int)_objects.size() - 1
    };
}

Json::Pointer
Json::Machine::NewList() {
    _lists.push_back(std::move(Json::list_t()));
    return Json::Pointer {
        Json::Type::LIST,
        (int)_lists.size() - 1
    };
}

#define DEFINE_JSONMACHINE_BUILDER(NAME, PARAM_TYPE, VECTOR, TYPE_SYMBOL) \
    Json::Pointer \
    Json::Machine::New##NAME(PARAM_TYPE value) { \
        VECTOR.push_back(value); \
        return Json::Pointer { \
            TYPE_SYMBOL, \
            (int)VECTOR.size() - 1 \
        }; \
    }

DEFINE_JSONMACHINE_BUILDER(String, const std::string &, _strings, Json::Type::STRING)
DEFINE_JSONMACHINE_BUILDER(Integer, int, _integers, Json::Type::INTEGER)
DEFINE_JSONMACHINE_BUILDER(Float, float, _floats, Json::Type::FLOAT)
DEFINE_JSONMACHINE_BUILDER(Boolean, bool, _booleans, Json::Type::BOOLEAN)
#undef DEFINE_JSONMACHINE_BUILDER

#define DEFINE_JSONMACHINE_GETTER(RETURN, NAME, RESOURCE) \
    RETURN & \
    Json::Machine::NAME(key_t key) { \
        return RESOURCE[key]; \
    }

DEFINE_JSONMACHINE_GETTER(Json::object_t, Object, _objects)
DEFINE_JSONMACHINE_GETTER(Json::list_t, List, _lists)
DEFINE_JSONMACHINE_GETTER(std::string, String, _strings)
DEFINE_JSONMACHINE_GETTER(int, Integer, _integers)
DEFINE_JSONMACHINE_GETTER(float, Float, _floats)
#undef DEFINE_JSONMACHINE_GETTER

void
Json::Machine::SetBoolean(key_t key, bool value) {
    _booleans.set((size_t)key, value);
}

#define DEFINE_JSONMACHINE_CONST_GETTER(RETURN, NAME, RESOURCE) \
    const RETURN & \
    Json::Machine::NAME(key_t key) const { \
        return RESOURCE.at(key); \
    }

DEFINE_JSONMACHINE_CONST_GETTER(Json::object_t, Object, _objects)
DEFINE_JSONMACHINE_CONST_GETTER(Json::list_t, List, _lists)
DEFINE_JSONMACHINE_CONST_GETTER(std::string, String, _strings)
DEFINE_JSONMACHINE_CONST_GETTER(int, Integer, _integers)
DEFINE_JSONMACHINE_CONST_GETTER(float, Float, _floats)
#undef DEFINE_JSONMACHINE_GETTER

bool
Json::Machine::Boolean(key_t key) const {
    return _booleans[(size_t)key];
}

std::string
Json::Machine::ToString() const {
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

const typename Json::Machine::ResultSet
Json::Machine::GetResultSet() const {
    ResultSet result;
    
    if (_objects.empty())
        result = ResultSet(this, Pointer{ Type::NIL, 0 });

    result = ResultSet(this, Pointer{ Type::OBJECT, (int)_objects.size() - 1 });
    return std::move(result);
}

Json::Machine::ResultSet::ResultSet(
    Json::Machine::machine_ptr_t machine,
    const Json::Pointer & pointer
):  _machine(machine),
    _pointer(pointer) {}

Json::Machine::ResultSet::ResultSet():
    _machine(nullptr),
    _pointer(Json::Pointer{ Json::Type::NIL, 0 }) {}

#define DEFINE_JSONMACHINE_RESULTSET_GETTER(NAME, TYPE, TYPE_SYMBOL) \
    bool \
    Json::Machine::ResultSet::As##NAME(TYPE & value) const { \
        if (_pointer.type != TYPE_SYMBOL) \
            return false; \
    \
        value = _machine->NAME(_pointer.key); \
        return true; \
    }

DEFINE_JSONMACHINE_RESULTSET_GETTER(Integer, int, Type::INTEGER)
DEFINE_JSONMACHINE_RESULTSET_GETTER(Float, float, Type::FLOAT)
DEFINE_JSONMACHINE_RESULTSET_GETTER(String, std::string, Type::STRING)
#undef DEFINE_JSONMACHINE_RESULTSET_GETTER

typename Json::Machine::ResultSet
Json::Machine::ResultSet::At(int index) const {
    if (_pointer.type != Type::LIST)
        return ResultSet(_machine, Pointer{ Type::NIL, 0 });

    return ResultSet(_machine, _machine->List(_pointer.key).at(index));
}

typename Json::Machine::ResultSet
Json::Machine::ResultSet::At(const std::string & key) const {
    if (_pointer.type != Type::OBJECT)
        return ResultSet(_machine, Pointer{ Type::NIL, 0 });

    return ResultSet(_machine, _machine->Object(_pointer.key).at(key));
}

typename Json::Machine::ResultSet
Json::Machine::ResultSet::operator[](int index) const {
    return At(index);
}

typename Json::Machine::ResultSet
Json::Machine::ResultSet::operator[](const std::string & key) const {
    return At(key);
}

std::string
Json::Machine::ResultSet::RecurseToString(
    Json::Pointer value
) const {
    std::ostringstream outss;

    switch (value.type) {
        case Type::STRING:
            outss
                << '"'
                << _machine->String(value.key)
                << '"';

            break;
        default:
            outss
                << ResultSet(_machine, value).ToString();

            break;
    }

    return outss.str();
}

Json::Type
Json::Machine::ResultSet::TypeCode() const {
    return _pointer.type;
}

bool
Json::Machine::ResultSet::IsNil() const {
    return _pointer.type == Type::NIL;
}

std::string
Json::Machine::ResultSet::ToString() const {
    std::ostringstream outss;

    switch (_pointer.type) {
        case Json::Type::STRING:
            return _machine->String(_pointer.key);
        case Json::Type::INTEGER:
            outss << _machine->Integer(_pointer.key);
            break;
        case Json::Type::FLOAT:
            outss << _machine->Float(_pointer.key);
            break;
        case Json::Type::BOOLEAN:
            outss << _machine->Boolean(_pointer.key);
            break;
        case Json::Type::OBJECT:
            {
                outss << "{ ";
                auto object = _machine->Object(_pointer.key);
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
                auto list = _machine->List(_pointer.key);

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
