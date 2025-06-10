#pragma once

#include <string>

using namespace std;

class Node;

class Component {
public:
    string name;
    Node *node1;
    Node *node2;

    Component() : name(""), node1(nullptr), node2(nullptr) {}
    Component(const std::string& name, Node* n1, Node* n2) : name(name), node1(n1), node2(n2) {}

    virtual double getCurrent() = 0;
    virtual double getVoltage() = 0;
    virtual void setCurrent(double c) {};
    virtual ~Component() {};
};