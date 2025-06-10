#pragma once

#include "Circuit.h"

void dcAnalysis(Circuit& circuit);
void transientAnalysis(Circuit& circuit, double t_step, double t_stop);
//void dsSweep(Circuit& circuit, double start_val, double end_val,double increment);