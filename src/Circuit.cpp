#include "Circuit.h"

using namespace std;

Circuit::Circuit() {}

Node *Circuit::findNode(const string &find_from_name) {
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

Resistor *Circuit::findResistor(const string &find_from_name, Node *n1, Node *n2) {
    if (find_from_name != "findbynode") {
        for (auto &res: resistors) {
            if (res.name == find_from_name) return &res;
        }
    }
    return nullptr;
}

Capacitor *Circuit::findCapacitor(const string &find_from_name, Node *n1, Node *n2) {
    if (find_from_name != "findbynode") {
        for (auto &cap: capacitors) {
            if (cap.name == find_from_name) return &cap;
        }
    }
    return nullptr;
}

Inductor *Circuit::findInductor(const string &find_from_name, Node *n1, Node *n2) {
    if (find_from_name != "findbynode") {
        for (auto &ind: inductors) {
            if (ind.name == find_from_name) return &ind;
        }
    }
    return nullptr;
}

Diode *Circuit::findDiode(const string &find_from_name, Node *n1, Node *n2) {
    if (find_from_name != "findbynode") {
        for (auto &dio: diodes) {
            if (dio.name == find_from_name) return &dio;
        }
    }
    return nullptr;
}


VoltageSource *Circuit::findVoltageSource(const string &find_from_name, Node *n1, Node *n2) {
    if (find_from_name != "findbynode") {
        for (auto &vs: voltageSources) {
            if (vs.name == find_from_name) return &vs;
        }
    }
    return nullptr;
}

CurrentSource *Circuit::findCurrentSource(const string &find_from_name, Node *n1, Node *n2) {
    if (find_from_name != "findbynode") {
        for (auto &cs: currentSources) {
            if (cs.name == find_from_name) return &cs;
        }
    }
    return nullptr;
}

vector<vector<double>> Circuit::G() { return {}; }

vector<vector<double>> Circuit::B() { return {}; }

vector<vector<double>> Circuit::C() { return {}; }

vector<vector<double>> Circuit::D() { return {}; }

vector<vector<double>> Circuit::J() { return {}; }

vector<vector<double>> Circuit::E() { return {}; }

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