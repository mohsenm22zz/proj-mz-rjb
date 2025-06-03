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

    virtual double getCurrent() = 0; // Pure virtual function
    virtual double getVoltage() = 0; // Pure virtual function
    virtual void setCurrent(double c) {}; // Virtual function (with default empty implementation)
    virtual ~Component() {}; // Virtual destructor for proper polymorphic deletion
};