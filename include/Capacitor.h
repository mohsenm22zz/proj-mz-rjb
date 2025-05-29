#pragma once

#include "Component.h" // Inherits from Component

class Capacitor : public Component {
public:
    double capacitance;
    double prevVoltage; // Voltage at the previous time step for transient analysis

    Capacitor() : capacitance(0.0), prevVoltage(0.0) {}

    double getCurrent() override;
    double getVoltage() override;
    void update(double dt);
};