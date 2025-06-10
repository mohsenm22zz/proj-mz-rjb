#pragma once

#include "Circuit.h"

void dcAnalysis(Circuit& circuit);
void transientAnalysis(Circuit& circuit, double t_step, double t_stop);
void dcSweepAnalysis(Circuit& circuit, const string& sourceName, double start, double end, double step);
