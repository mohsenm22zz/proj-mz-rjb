#pragma once
#include <string>

using namespace std;
class Node {
public:
    string name;
    int num;
    static int nextNum;
    double voltage;
    bool isGround;

    Node();
    double getVoltage() const;
    void setVoltage(double v);
    void setGround(bool ground_status);
};