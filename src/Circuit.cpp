#include "Circuit.h"

Circuit::Circuit() {}

Node* Circuit::findNode(const std::string& find_from_name) {
    for (auto &node : nodes) {
        if (node.name == find_from_name) return &node;
    }
    return nullptr;
}

Node* Circuit::findNodeByNum(int num_to_find) {
    for (auto &node : nodes) {
        if (node.num == num_to_find) return &node;
    }
    return nullptr;
}

Resistor* Circuit::findResistor(const std::string& find_from_name) {
    for (auto &res : resistors) {
        if (res.name == find_from_name) return &res;
    }
    return nullptr;
}

Capacitor* Circuit::findCapacitor(const std::string& find_from_name) {
    for (auto &cap : capacitors) {
        if (cap.name == find_from_name) return &cap;
    }
    return nullptr;
}

Inductor* Circuit::findInductor(const std::string& find_from_name) {
    for (auto &ind : inductors) {
        if (ind.name == find_from_name) return &ind;
    }
    return nullptr;
}

Diode* Circuit::findDiode(const std::string& find_from_name) {
    for (auto &dio : diodes) {
        if (dio.name == find_from_name) return &dio;
    }
    return nullptr;
}

CurrentSource* Circuit::findCurrentSource(const std::string& find_from_name) {
    for (auto &cs : currentSources) {
        if (cs.name == find_from_name) return &cs;
    }
    return nullptr;
}

std::vector<std::vector<double>> Circuit::G(){ return {}; }
std::vector<std::vector<double>> Circuit::B(){ return {}; }
std::vector<std::vector<double>> Circuit::C(){ return {}; }
std::vector<std::vector<double>> Circuit::D(){ return {}; }
std::vector<std::vector<double>> Circuit::J(){ return {}; }
std::vector<std::vector<double>> Circuit::E(){ return {}; }

void Circuit::set_MNA_A(){}
void Circuit::set_MNA_v(){}
void Circuit::set_MNA_x(){}

void Circuit::addNode(const std::string& name) {
    if (!findNode(name)) {
        Node newNode;
        newNode.name = name;
        nodes.push_back(newNode);
    }
}