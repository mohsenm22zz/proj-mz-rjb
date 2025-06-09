#pragma once

#include "Circuit.h"

void dcAnalysis(Circuit& circuit);
void transientAnalysis(Circuit& circuit, double t_step, double t_stop);