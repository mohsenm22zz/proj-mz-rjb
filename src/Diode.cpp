#include "Diode.h"
#include "Node.h"
#include <cmath>

const double ON_RESISTANCE = 1e-3;
const double OFF_RESISTANCE = 1e12;

Diode::Diode(const std::string& name, Node* n1, Node* n2, DiodeType type, double vf, double vz)
        : Component(name, n1, n2),
          diodeType(type),
          currentState(STATE_OFF),
          forwardVoltage(vf),
          zenerVoltage(vz),
          branchIndex(-1)
{
}

DiodeType Diode::getDiodeType() const {
    return diodeType;
}

double Diode::getForwardVoltage() const {
    return forwardVoltage;
}

double Diode::getZenerVoltage() const {
    return zenerVoltage;
}

void Diode::setState(DiodeState state) {
    currentState = state;
}

DiodeState Diode::getState() const {
    return currentState;
}

void Diode::setBranchIndex(int index) {
    branchIndex = index;
}

int Diode::getBranchIndex() const {
    return branchIndex;
}

// Implementation for the pure virtual functions inherited from Component
double Diode::getCurrent() {
    if (!node1 || !node2) return 0.0;

    double v_diode = node1->getVoltage() - node2->getVoltage();

    switch (currentState) {
        case STATE_OFF:
            // Very small current when off (due to large off-resistance)
            return v_diode / OFF_RESISTANCE;
        case STATE_FORWARD_ON:
            // Current based on forward voltage drop and on-resistance
            return (v_diode - forwardVoltage) / ON_RESISTANCE;
        case STATE_REVERSE_ON:
            // Current based on Zener voltage drop and on-resistance
            // Note: Zener current flows in reverse direction, so voltage is node2-node1, or -(node1-node2)
            return (v_diode - (-zenerVoltage)) / ON_RESISTANCE; // (v_diode + zenerVoltage) / ON_RESISTANCE
        default:
            return 0.0;
    }
}

double Diode::getVoltage() {
    if (!node1 || !node2) return 0.0;
    // Return the absolute voltage difference across the diode's terminals
    return fabs(node1->getVoltage() - node2->getVoltage());
}

void Diode::addStamp(std::vector<std::vector<double>>& A, std::vector<double>& b, double t) {
    // This function is intentionally left empty as diode stamping logic is handled in the DC analysis function.
}