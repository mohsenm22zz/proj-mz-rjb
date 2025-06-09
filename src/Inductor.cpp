#include "Inductor.h"
#include "Node.h"
#include <cmath>

using namespace std;

double Inductor::getCurrent() {
    return current;
}

double Inductor::getVoltage() {
    // In MNA, voltage is handled by the system of equations, not directly here.
    return 0.0;
}

void Inductor::update(double dt) {
    if (!node1 || !node2) return;
    prevCurrent = current;
}

void Inductor::setInductorCurrent(double c) {
    this->current = c;
}