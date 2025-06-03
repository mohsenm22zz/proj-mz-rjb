#include "VoltageSource.h"

using namespace std;

double VoltageSource::getCurrent() {
    return current;
}

void VoltageSource::setCurrent(double c) {
    current = c;
}

double VoltageSource::getVoltage() {
    return value;
}