#include "Capacitor.h"
#include "Node.h"
#include <cmath>

double Capacitor::getCurrent() {
    return 0.0;
}

double Capacitor::getVoltage() {
    if (!node1 || !node2) return 0.0;
    return std::fabs(node1->getVoltage() - node2->getVoltage());
}

void Capacitor::update(double dt) {
    if (!node1 || !node2) return;
    prevVoltage = node1->getVoltage() - node2->getVoltage();
}