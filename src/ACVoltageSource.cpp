#include "ACVoltageSource.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

double ACVoltageSource::getValue(double time) const {
    double phase_rad = phase * M_PI / 180.0;
    return magnitude * sin(2 * M_PI * frequency * time + phase_rad);
}
