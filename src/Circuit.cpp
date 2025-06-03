#include "Circuit.h"

using namespace std;

Circuit::Circuit() {}

Node *Circuit::findNode(const std::string &find_from_name) {
    for (auto &node: nodes) {
        if (node.name == find_from_name) return &node;
    }
    return nullptr;
}

Node *Circuit::findNodeByNum(int num_to_find) {
    for (auto &node: nodes) {
        if (node.num == num_to_find) return &node;
    }
    return nullptr;
}

Resistor *Circuit::findResistor(const std::string &find_from_name) {
    for (auto &res: resistors) {
        if (res.name == find_from_name) return &res;
    }
    return nullptr;
}

Capacitor *Circuit::findCapacitor(const std::string &find_from_name) {
    for (auto &cap: capacitors) {
        if (cap.name == find_from_name) return &cap;
    }
    return nullptr;
}

Inductor *Circuit::findInductor(const std::string &find_from_name) {
    for (auto &ind: inductors) {
        if (ind.name == find_from_name) return &ind;
    }
    return nullptr;
}

Diode *Circuit::findDiode(const std::string &find_from_name) {
    for (auto &dio: diodes) {
        if (dio.name == find_from_name) return &dio;
    }
    return nullptr;
}

CurrentSource *Circuit::findCurrentSource(const std::string &find_from_name) {
    for (auto &cs: currentSources) {
        if (cs.name == find_from_name) return &cs;
    }
    return nullptr;
}


vector<vector<double>> Circuit::G() {
    vector<vector<double>> result;
    for (Node &n1: nodes) {
        for (Node &n2: nodes) {
            if (&n1 != &n2) {
/// find component n1, n2, &type
            }
        }
    }
    return result;
}

vector<vector<double>> Circuit::B() {
    vector<vector<double>> result;
    return result;
}

vector<vector<double>> Circuit::C() {
    vector<vector<double>> result;
    return result;
}

vector<vector<double>> Circuit::D() {
    vector<vector<double>> result;
    return result;
}

vector<vector<double>> Circuit::J() {
    vector<vector<double>> result;
    return result;
}

vector<vector<double>> Circuit::E() {
    vector<vector<double>> result;
    return result;
}

void Circuit::set_MNA_A() {}

void Circuit::set_MNA_v() {}

void Circuit::set_MNA_x() {}

void Circuit::addNode(const string &name) {
    if (!findNode(name)) {
        Node newNode;
        newNode.name = name;
        nodes.push_back(newNode);
    }
}