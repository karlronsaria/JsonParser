#include "JsonTreeDemo002.h"

void MyJsonMachineTreeVisitor::StartObject() {
    // TODO: empty stack check
    auto top = _objects.top();
    _objects.push(_machine->NewObject());
}

void MyJsonMachineTreeVisitor::EndObject() {
}

void MyJsonMachineTreeVisitor::StartList() {
}

void MyJsonMachineTreeVisitor::EndList() {
}

void MyJsonMachineTreeVisitor::StartPair(const std::string & key) {
}

void MyJsonMachineTreeVisitor::EndPair() {
}

void MyJsonMachineTreeVisitor::StartValue() {
}

void MyJsonMachineTreeVisitor::EndValue() {
}

void MyJsonMachineTreeVisitor::ForNumeric(Homonumeric value) {
    switch (value.Mode) {
        case Homonumeric::Mode::BOOLEAN:
            break;
        case Homonumeric::Mode::INTEGER:
            break;
        case Homonumeric::Mode::FLOAT:
            break;
        default:
            break;
    }
}

void MyJsonMachineTreeVisitor::ForString(const std::string & value) {
}
