#pragma once

#include "export.h"
#include "Circuit.h"
#include <vector>
#include <string>

extern "C" {
    // Function to create a new circuit
    CIRCUITSIMULATOR_API void* CreateCircuit();

    // Function to destroy a circuit
    CIRCUITSIMULATOR_API void DestroyCircuit(void* circuit);

    // Function to add a node to the circuit
    CIRCUITSIMULATOR_API void AddNode(void* circuit, const char* name);

    // Function to add a resistor to the circuit
    CIRCUITSIMULATOR_API void AddResistor(void* circuit, const char* name, const char* node1, const char* node2, double value);

    // Function to add a voltage source to the circuit
    CIRCUITSIMULATOR_API void AddVoltageSource(void* circuit, const char* name, const char* node1, const char* node2, double voltage);

    // Function to set ground node
    CIRCUITSIMULATOR_API void SetGroundNode(void* circuit, const char* nodeName);

    // Function to run DC analysis
    CIRCUITSIMULATOR_API bool RunDCAnalysis(void* circuit);

    // Function to run transient analysis
    CIRCUITSIMULATOR_API bool RunTransientAnalysis(void* circuit, double stepTime, double stopTime);

    // Function to get node voltage
    CIRCUITSIMULATOR_API double GetNodeVoltage(void* circuit, const char* nodeName);

    // Function to get all node names
    CIRCUITSIMULATOR_API int GetNodeNames(void* circuit, const char** nodeNames, int maxCount);

    // Function to get node voltages for transient analysis
    CIRCUITSIMULATOR_API int GetNodeVoltageHistory(void* circuit, const char* nodeName, double* timePoints, double* voltages, int maxCount);
}