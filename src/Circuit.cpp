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

Resistor *Circuit::findResistor(const string &find_from_name) {
    for (auto &res: resistors) {
        if (res.name == find_from_name) return &res;
    }
    return nullptr;
}

Capacitor *Circuit::findCapacitor(const string &find_from_name) {
    for (auto &cap: capacitors) {
        if (cap.name == find_from_name) return &cap;
    }
    return nullptr;
}

Inductor *Circuit::findInductor(const string &find_from_name) {
    for (auto &ind: inductors) {
        if (ind.name == find_from_name) return &ind;
    }
    return nullptr;
}

Diode *Circuit::findDiode(const string &find_from_name) {
    for (auto &dio: diodes) {
        if (dio.name == find_from_name) return &dio;
    }
    return nullptr;
}

CurrentSource *Circuit::findCurrentSource(const string &find_from_name) {
    for (auto &cs: currentSources) {
        if (cs.name == find_from_name) return &cs;
    }
    return nullptr;
}


vector<Component> *Circuit::findComponent(Node *n1, Node *n2, string &type) {/// تایپ برای تعیین اینکه کدوم جز هست
    /// المان های بین دو گره
    ///تایپ ها:
    ///r,c,i,d,vs.cs
    return nullptr;
}

vector<vector<double>> Circuit::G() {/// ماتریس هدایت و خازن و سلف
/// C/Delta t , Delta t/L ضرب میشوند
    vector<vector<double>> result;
    for (Node &n1: nodes) {
        for (Node &n2: nodes) {
            if (&n1 != &n2) {/// اگر مساوی بودند المان هایی که یک سرشون گره اول هست
/// پیدا کردن المان
/// findcomponent
/// if و else ها برای static cast کردن به نوع
            }
        }
    }
    return result;
}

vector<vector<double>> Circuit::B() {/// 1,0,-1
/// برای ولتاژ ها
    vector<vector<double>> result;
    return result;
}

vector<vector<double>> Circuit::C() {/// ترانهاده قبلی
    vector<vector<double>> result;
    return result;
}

vector<vector<double>> Circuit::D() {/// منابع وابسته
    vector<vector<double>> result;
    return result;
}

vector<vector<double>> Circuit::J() {///
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