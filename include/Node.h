#pragma once
#include <string>

class Node {
public:
    std::string name;
    int num;
    static int nextNum;
    double voltage;
    bool isGround;

    Node();
    double getVoltage() const;
    void setVoltage(double v);
    void setGround(bool ground_status);
};