#pragma once

#include "Circuit.h"

void dcAnalysis(Circuit& circuit);
void transientAnalysis(Circuit& circuit, double tstep, double tstop);