// mohsenm22zz/proj-mz-rjb/proj-mz-rjb-1b949d5aa204b9f590a1c5f0644f3424cf2a70ce/include/CircuitSimulatorInterface.h

#pragma once

#include "export.h" // Macro for DLL export/import
#include "Circuit.h"
#include <vector>
#include <string>

// C-style interface to be exported from the DLL.
// This allows the C++ code to be called from other languages like C#.
extern "C" {
    CIRCUITSIMULATOR_API void* CreateCircuit();
    CIRCUITSIMULATOR_API void DestroyCircuit(void* circuit);
    CIRCUITSIMULATOR_API void AddNode(void* circuit, const char* name);
    CIRCUITSIMULATOR_API void AddResistor(void* circuit, const char* name, const char* node1, const char* node2, double value);
    CIRCUITSIMULATOR_API void AddVoltageSource(void* circuit, const char* name, const char* node1, const char* node2, double voltage);
    
    // --- NEW: Added missing function declarations to match C# wrapper ---
    CIRCUITSIMULATOR_API void AddACVoltageSource(void* circuit, const char* name, const char* node1, const char* node2, double magnitude, double phase);

    CIRCUITSIMULATOR_API void SetGroundNode(void* circuit, const char* nodeName);
    CIRCUITSIMULATOR_API bool RunDCAnalysis(void* circuit);
    CIRCUITSIMULATOR_API bool RunTransientAnalysis(void* circuit, double stepTime, double stopTime);

    // --- NEW: Added missing function declarations to match C# wrapper ---
    CIRCUITSIMULATOR_API bool RunACAnalysis(void* circuit, const char* sourceName, double startFreq, double stopFreq, int numPoints, const char* sweepType);

    CIRCUITSIMULATOR_API double GetNodeVoltage(void* circuit, const char* nodeName);

    // --- FIX: Changed signature to write into a character buffer for safe string marshalling ---
    CIRCUITSIMULATOR_API int GetNodeNames(void* circuit, char* nodeNamesBuffer, int bufferSize);

    CIRCUITSIMULATOR_API int GetNodeVoltageHistory(void* circuit, const char* nodeName, double* timePoints, double* voltages, int maxCount);

    // --- NEW: Added missing function declarations to match C# wrapper ---
    CIRCUITSIMULATOR_API int GetNodeSweepHistory(void* circuit, const char* nodeName, double* frequencies, double* magnitudes, int maxCount);
}
