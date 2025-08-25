#pragma once

#include "VoltageSource.h"
#include <complex>

// --- NEW FILE ---
// Represents a voltage source for AC analysis.
class ACVoltageSource : public VoltageSource {
public:
    double acMagnitude;
    double acPhase; // Phase in radians

    // The frequency (omega) will be set by the analysis function during a sweep.
    // The 'value' from the base class can be used for DC offset if needed.
    ACVoltageSource() : acMagnitude(1.0), acPhase(0.0) {
        // Default to 1V magnitude, 0 phase
    }

    // Returns the complex phasor value for AC analysis
    std::complex<double> getPhasor() const {
        return std::polar(acMagnitude, acPhase);
    }
};
