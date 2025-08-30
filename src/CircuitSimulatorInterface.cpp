#include "CircuitSimulatorInterface.h"
#include "Analysis.h"
#include <cstring>
#include <string>
#include <sstream>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

extern "C" {
    void* CreateCircuit() {
        try {
            return new Circuit();
        }
        catch (...) {
            return nullptr;
        }
    }

    void DestroyCircuit(void* circuit) {
        if (circuit) {
            delete static_cast<Circuit*>(circuit);
        }
    }

    int AddNode(void* circuit, const char* name) {
        if (!circuit || !name) {
            return CIRCUIT_SIM_ERROR_INVALID_ARGUMENT;
        }
        
        try {
            static_cast<Circuit*>(circuit)->addNode(name);
            return CIRCUIT_SIM_SUCCESS;
        }
        catch (...) {
            return CIRCUIT_SIM_ERROR_ANALYSIS_FAILED;
        }
    }

    int AddResistor(void* circuit, const char* name, const char* node1, const char* node2, double value) {
        if (!circuit || !name || !node1 || !node2) {
            return CIRCUIT_SIM_ERROR_INVALID_ARGUMENT;
        }
        
        if (value <= 0) {
            return CIRCUIT_SIM_ERROR_INVALID_ARGUMENT;
        }
        
        try {
            Circuit* c = static_cast<Circuit*>(circuit);
            Node* n1 = c->findOrCreateNode(node1);
            Node* n2 = c->findOrCreateNode(node2);
            c->resistors.emplace_back();
            Resistor& resistor = c->resistors.back();
            resistor.name = name;
            resistor.node1 = n1;
            resistor.node2 = n2;
            resistor.resistance = value;
            return CIRCUIT_SIM_SUCCESS;
        }
        catch (...) {
            return CIRCUIT_SIM_ERROR_ANALYSIS_FAILED;
        }
    }

    int AddVoltageSource(void* circuit, const char* name, const char* node1, const char* node2, double voltage) {
        if (!circuit || !name || !node1 || !node2) {
            return CIRCUIT_SIM_ERROR_INVALID_ARGUMENT;
        }
        
        try {
            Circuit* c = static_cast<Circuit*>(circuit);
            Node* n1 = c->findOrCreateNode(node1);
            Node* n2 = c->findOrCreateNode(node2);
            c->voltageSources.emplace_back();
            VoltageSource& vs = c->voltageSources.back();
            vs.name = name;
            vs.node1 = n1;
            vs.node2 = n2;
            vs.value = voltage;
            return CIRCUIT_SIM_SUCCESS;
        }
        catch (...) {
            return CIRCUIT_SIM_ERROR_ANALYSIS_FAILED;
        }
    }
    
    int AddACVoltageSource(void* circuit, const char* name, const char* node1, const char* node2, double magnitude, double phase) {
        if (!circuit || !name || !node1 || !node2) {
            return CIRCUIT_SIM_ERROR_INVALID_ARGUMENT;
        }
        
        if (magnitude < 0) {
            return CIRCUIT_SIM_ERROR_INVALID_ARGUMENT;
        }
        
        try {
            Circuit* c = static_cast<Circuit*>(circuit);
            Node* n1 = c->findOrCreateNode(node1);
            Node* n2 = c->findOrCreateNode(node2);
            c->acVoltageSources.emplace_back();
            ACVoltageSource& vs = c->acVoltageSources.back();
            vs.name = name;
            vs.node1 = n1;
            vs.node2 = n2;
            vs.magnitude = magnitude;
            vs.phase = phase * M_PI / 180.0; // Convert to radians
            return CIRCUIT_SIM_SUCCESS;
        }
        catch (...) {
            return CIRCUIT_SIM_ERROR_ANALYSIS_FAILED;
        }
    }

    int SetGroundNode(void* circuit, const char* nodeName) {
        if (!circuit || !nodeName) {
            return CIRCUIT_SIM_ERROR_INVALID_ARGUMENT;
        }
        
        try {
            Circuit* c = static_cast<Circuit*>(circuit);
            Node* node = c->findOrCreateNode(nodeName);
            node->setGround(true);
            return CIRCUIT_SIM_SUCCESS;
        }
        catch (...) {
            return CIRCUIT_SIM_ERROR_ANALYSIS_FAILED;
        }
    }

    int RunDCAnalysis(void* circuit) {
        if (!circuit) {
            return CIRCUIT_SIM_ERROR_INVALID_ARGUMENT;
        }
        
        try {
            dcAnalysis(*static_cast<Circuit*>(circuit));
            return CIRCUIT_SIM_SUCCESS;
        }
        catch (...) {
            return CIRCUIT_SIM_ERROR_ANALYSIS_FAILED;
        }
    }

    int RunTransientAnalysis(void* circuit, double stepTime, double stopTime) {
        if (!circuit) {
            return CIRCUIT_SIM_ERROR_INVALID_ARGUMENT;
        }
        
        if (stepTime <= 0 || stopTime <= 0 || stepTime > stopTime) {
            return CIRCUIT_SIM_ERROR_INVALID_ARGUMENT;
        }
        
        try {
            transientAnalysis(*static_cast<Circuit*>(circuit), stepTime, stopTime);
            return CIRCUIT_SIM_SUCCESS;
        }
        catch (...) {
            return CIRCUIT_SIM_ERROR_ANALYSIS_FAILED;
        }
    }

    int RunACAnalysis(void* circuit, const char* sourceName, double startFreq, double stopFreq, int numPoints, const char* sweepType) {
        if (!circuit || !sourceName) {
            return CIRCUIT_SIM_ERROR_INVALID_ARGUMENT;
        }
        
        if (startFreq <= 0 || stopFreq <= 0 || startFreq > stopFreq || numPoints <= 0) {
            return CIRCUIT_SIM_ERROR_INVALID_ARGUMENT;
        }
        
        // Use "LIN" as default sweep type if not provided
        std::string sweep = sweepType ? sweepType : "LIN";
        
        try {
            acSweepAnalysis(*static_cast<Circuit*>(circuit), sourceName, startFreq, stopFreq, numPoints, sweep);
            return CIRCUIT_SIM_SUCCESS;
        }
        catch (...) {
            return CIRCUIT_SIM_ERROR_ANALYSIS_FAILED;
        }
    }
    
    int RunPhaseAnalysis(void* circuit, const char* sourceName, double baseFreq, double startPhase, double stopPhase, int numPoints) {
        if (!circuit || !sourceName) {
            return CIRCUIT_SIM_ERROR_INVALID_ARGUMENT;
        }
        
        if (baseFreq <= 0 || numPoints <= 0) {
            return CIRCUIT_SIM_ERROR_INVALID_ARGUMENT;
        }
        
        try {
            phaseSweepAnalysis(*static_cast<Circuit*>(circuit), sourceName, baseFreq, startPhase, stopPhase, numPoints);
            return CIRCUIT_SIM_SUCCESS;
        }
        catch (...) {
            return CIRCUIT_SIM_ERROR_ANALYSIS_FAILED;
        }
    }

    int GetNodeVoltage(void* circuit, const char* nodeName, double* voltage) {
        if (!circuit || !nodeName || !voltage) {
            return CIRCUIT_SIM_ERROR_INVALID_ARGUMENT;
        }
        
        Node* node = static_cast<Circuit*>(circuit)->findNode(nodeName);
        if (!node) {
            return CIRCUIT_SIM_ERROR_NOT_FOUND;
        }
        
        *voltage = node->getVoltage();
        return CIRCUIT_SIM_SUCCESS;
    }

    int GetNodeNames(void* circuit, char* nodeNamesBuffer, int bufferSize) {
        if (!circuit || !nodeNamesBuffer || bufferSize <= 0) {
            return CIRCUIT_SIM_ERROR_INVALID_ARGUMENT;
        }
        
        Circuit* c = static_cast<Circuit*>(circuit);
        std::stringstream ss;
        bool first = true;
        for (const auto& node : c->nodes) {
            if (!node->isGround) {
                if (!first) ss << ",";
                ss << node->name;
                first = false;
            }
        }
        std::string allNames = ss.str();
        if (allNames.length() < static_cast<size_t>(bufferSize)) {
            strcpy_s(nodeNamesBuffer, bufferSize, allNames.c_str());
            return static_cast<int>(allNames.length() + 1);
        }
        nodeNamesBuffer[0] = '\0';
        return 0;
    }

    int GetNodeVoltageHistory(void* circuit, const char* nodeName, double* timePoints, double* voltages, int maxCount) {
        if (!circuit || !nodeName || !timePoints || !voltages || maxCount <= 0) {
            return CIRCUIT_SIM_ERROR_INVALID_ARGUMENT;
        }
        
        Node* node = static_cast<Circuit*>(circuit)->findNode(nodeName);
        if (!node) {
            return CIRCUIT_SIM_ERROR_NOT_FOUND;
        }
        
        int count = 0;
        for (const auto& point : node->voltage_history) {
            if (count >= maxCount) break;
            timePoints[count] = point.first;
            voltages[count] = point.second;
            count++;
        }
        return count;
    }

    int GetNodeSweepHistory(void* circuit, const char* nodeName, double* frequencies, double* magnitudes, int maxCount) {
        if (!circuit || !nodeName || !frequencies || !magnitudes || maxCount <= 0) {
            return CIRCUIT_SIM_ERROR_INVALID_ARGUMENT;
        }
        
        Node* node = static_cast<Circuit*>(circuit)->findNode(nodeName);
        if (!node) {
            return CIRCUIT_SIM_ERROR_NOT_FOUND;
        }
        
        int count = 0;
        for (const auto& point : node->ac_sweep_history) {
            if (count >= maxCount) break;
            frequencies[count] = point.first;
            magnitudes[count] = point.second;
            count++;
        }
        return count;
    }
    
    int GetNodePhaseSweepHistory(void* circuit, const char* nodeName, double* phases, double* magnitudes, int maxCount) {
        if (!circuit || !nodeName || !phases || !magnitudes || maxCount <= 0) {
            return CIRCUIT_SIM_ERROR_INVALID_ARGUMENT;
        }
        
        Node* node = static_cast<Circuit*>(circuit)->findNode(nodeName);
        if (!node) {
            return CIRCUIT_SIM_ERROR_NOT_FOUND;
        }
        
        int count = 0;
        for (const auto& point : node->phase_sweep_history) {
            if (count >= maxCount) break;
            phases[count] = point.first;
            magnitudes[count] = point.second;
            count++;
        }
        return count;
    }

    int GetComponentCurrentHistory(void* circuit, const char* componentName, double* timePoints, double* currents, int maxCount) {
        if (!circuit || !componentName || !timePoints || !currents || maxCount <= 0) {
            return CIRCUIT_SIM_ERROR_INVALID_ARGUMENT;
        }
        
        Circuit* c = static_cast<Circuit*>(circuit);
        VoltageSource* vs = c->findVoltageSource(componentName);
        if (!vs) {
            return CIRCUIT_SIM_ERROR_NOT_FOUND;
        }
        
        int count = 0;
        for (const auto& point : vs->current_history) {
            if (count >= maxCount) break;
            timePoints[count] = point.first;
            currents[count] = point.second;
            count++;
        }
        return count;
    }

    // --- NEW: Implementation for getting all voltage source names ---
    int GetAllVoltageSourceNames(void* circuit, char* vsNamesBuffer, int bufferSize) {
        if (!circuit || !vsNamesBuffer || bufferSize <= 0) {
            return CIRCUIT_SIM_ERROR_INVALID_ARGUMENT;
        }
        
        Circuit* c = static_cast<Circuit*>(circuit);
        std::stringstream ss;
        bool first = true;
        for (const auto& vs : c->voltageSources) {
            if (!first) ss << ",";
            ss << vs.name;
            first = false;
        }
        std::string allNames = ss.str();
        if (allNames.length() < static_cast<size_t>(bufferSize)) {
            strcpy_s(vsNamesBuffer, bufferSize, allNames.c_str());
            return static_cast<int>(allNames.length() + 1);
        }
        vsNamesBuffer[0] = '\0';
        return 0;
    }

    int GetVoltageSourceCurrent(void* circuit, const char* vsName, double* current) {
        if (!circuit || !vsName || !current) {
            return CIRCUIT_SIM_ERROR_INVALID_ARGUMENT;
        }
        
        VoltageSource* vs = static_cast<Circuit*>(circuit)->findVoltageSource(vsName);
        if (!vs) {
            return CIRCUIT_SIM_ERROR_NOT_FOUND;
        }
        
        *current = vs->getCurrent();
        return CIRCUIT_SIM_SUCCESS;
    }
}