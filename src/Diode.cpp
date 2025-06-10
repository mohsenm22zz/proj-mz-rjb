#include "Diode.h"

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

void Diode::addStamp(std::vector<std::vector<double>>& A, std::vector<double>& b, double t) {
    // این تابع عمداً خالی گذاشته شده است
    // منطق تمبر زنی دیودها در تابع تحلیل DC مدیریت می‌شود
}
