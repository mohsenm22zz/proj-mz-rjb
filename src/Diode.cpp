#include "Diode.h"
#include "Node.h"
#include <cmath>
double Diode::getCurrent() {
    if (!node1 || !node2) return 0.0;
    double Vd = node1->getVoltage() - node2->getVoltage();

    double exponent_val = Vd / (n * Vt);
    if (exponent_val > 700) {
        return Is * (std::exp(700.0) - 1.0);
    } else if (exponent_val < -700) {
        return -Is;
    }
    return Is * (std::exp(exponent_val) - 1.0);
}

double Diode::getVoltage() {
    if (!node1 || !node2) return 0.0;
    return std::fabs(node1->getVoltage() - node2->getVoltage());
}