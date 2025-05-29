#include "CurrentSource.h"
#include "Node.h"
#include <cmath>

double CurrentSource::getCurrent() {
    return value;
}

double CurrentSource::getVoltage() {
    if (!node1 || !node2) return 0.0;
    return std::fabs(node1->getVoltage() - node2->getVoltage());
}