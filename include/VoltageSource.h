#pragma once

#include "Component.h"

class VoltageSource : public Component {
public:
    double value;
    double current;

    VoltageSource() : value(0.0), current(0.0) {}

    double getCurrent() override;
    void setCurrent(double c) override;
    double getVoltage() override;
};