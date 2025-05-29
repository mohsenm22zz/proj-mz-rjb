#include "Resistor.h"
#include "Node.h"
#include <cmath>
#include <limits>

double Resistor::getCurrent() {
    if (resistance == 0) return std::numeric_limits<double>::infinity(); /// error بدهد
    if (!node1 || !node2) return 0.0;
    return std::fabs(node1->getVoltage() - node2->getVoltage()) / resistance;
}

double Resistor::getVoltage() {
    if (!node1 || !node2) return 0.0;
    return std::fabs(node1->getVoltage() - node2->getVoltage());
}