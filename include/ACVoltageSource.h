#pragma once

#include "VoltageSource.h"
#include <complex>

class ACVoltageSource : public VoltageSource {
public:
    double acMagnitude;
    double acPhase; //radian
    ACVoltageSource() : acMagnitude(1.0), acPhase(0.0) {
    }

    // Returns the complex phasor value for AC analysis
    std::complex<double> getPhasor() const {
        return std::polar(acMagnitude, acPhase);
    }
};
