#pragma once

#include "Circuit.h"
#include "export.h" // Include the export header

// Add the export macro to the function declarations that you want to call from C#
CIRCUITSIMULATOR_API void dcAnalysis(Circuit& circuit);
CIRCUITSIMULATOR_API void transientAnalysis(Circuit& circuit, double t_step, double t_stop);
CIRCUITSIMULATOR_API void dcSweepAnalysis(Circuit& circuit, const std::string& sourceName, double start, double end, double step);
