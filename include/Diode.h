#pragma once

#include "Component.h"
#include <cmath>
class Diode : public Component {
public:
    double Is;
    double Vt;
    double n;

    Diode() : Is(1e-12), Vt(0.026), n(1.0) {}

    double getCurrent() override;
    double getVoltage() override;
};