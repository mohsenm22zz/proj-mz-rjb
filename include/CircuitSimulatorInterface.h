#pragma once

#include "export.h"
#include "Circuit.h"
#include <vector>
#include <string>

#define CIRCUIT_SIM_SUCCESS 0
#define CIRCUIT_SIM_ERROR_INVALID_ARGUMENT -1
#define CIRCUIT_SIM_ERROR_NOT_FOUND -2
#define CIRCUIT_SIM_ERROR_ANALYSIS_FAILED -3

extern "C" {
    CIRCUITSIMULATOR_API void* CreateCircuit();
    CIRCUITSIMULATOR_API void DestroyCircuit(void* circuit);
    CIRCUITSIMULATOR_API int AddNode(void* circuit, const char* name);
    CIRCUITSIMULATOR_API int AddResistor(void* circuit, const char* name, const char* node1, const char* node2, double value);
    CIRCUITSIMULATOR_API int AddVoltageSource(void* circuit, const char* name, const char* node1, const char* node2, double voltage);
    CIRCUITSIMULATOR_API int AddACVoltageSource(void* circuit, const char* name, const char* node1, const char* node2, double magnitude, double phase);
    CIRCUITSIMULATOR_API int SetGroundNode(void* circuit, const char* nodeName);
    CIRCUITSIMULATOR_API int RunDCAnalysis(void* circuit);
    CIRCUITSIMULATOR_API int RunTransientAnalysis(void* circuit, double stepTime, double stopTime);
    CIRCUITSIMULATOR_API int RunACAnalysis(void* circuit, const char* sourceName, double startFreq, double stopFreq, int numPoints, const char* sweepType);
    CIRCUITSIMULATOR_API int GetNodeVoltage(void* circuit, const char* nodeName, double* voltage);
    CIRCUITSIMULATOR_API int GetNodeNames(void* circuit, char* nodeNamesBuffer, int bufferSize);
    CIRCUITSIMULATOR_API int GetNodeVoltageHistory(void* circuit, const char* nodeName, double* timePoints, double* voltages, int maxCount);
    CIRCUITSIMULATOR_API int GetNodeSweepHistory(void* circuit, const char* nodeName, double* frequencies, double* magnitudes, int maxCount);
    CIRCUITSIMULATOR_API int GetComponentCurrentHistory(void* circuit, const char* componentName, double* timePoints, double* currents, int maxCount);
    CIRCUITSIMULATOR_API int GetAllVoltageSourceNames(void* circuit, char* vsNamesBuffer, int bufferSize);
    CIRCUITSIMULATOR_API int GetVoltageSourceCurrent(void* circuit, const char* vsName, double* current);
}