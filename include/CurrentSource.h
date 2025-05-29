#pragma once

#include "Component.h" // Inherits from Component

class CurrentSource : public Component {
public:
    double value; // Current value of the source

    CurrentSource() : value(0.0) {}

    double getCurrent() override;
    double getVoltage() override;
};