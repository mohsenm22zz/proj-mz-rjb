// mohsenm22zz/proj-mz-rjb/proj-mz-rjb-1b949d5aa204b9f590a1c5f0644f3424cf2a70ce/src/CircuitSimulatorInterface.cpp

#include "CircuitSimulatorInterface.h"
#include "Analysis.h"
#include <cstring> // For strcpy_s
#include <string>
#include <sstream> // For std::stringstream

extern "C" {
    void* CreateCircuit() {
        return new Circuit();
    }

    void DestroyCircuit(void* circuit) {
        if (circuit) {
            delete static_cast<Circuit*>(circuit);
        }
    }

    void AddNode(void* circuit, const char* name) {
        if (circuit && name) {
            static_cast<Circuit*>(circuit)->addNode(name);
        }
    }

    void AddResistor(void* circuit, const char* name, const char* node1, const char* node2, double value) {
        if (circuit && name && node1 && node2) {
            Circuit* c = static_cast<Circuit*>(circuit);
            Node* n1 = c->findOrCreateNode(node1);
            Node* n2 = c->findOrCreateNode(node2);
            c->resistors.emplace_back();
            Resistor& resistor = c->resistors.back();
            resistor.name = name;
            resistor.node1 = n1;
            resistor.node2 = n2;
            resistor.resistance = value;
        }
    }

    void AddVoltageSource(void* circuit, const char* name, const char* node1, const char* node2, double voltage) {
        if (circuit && name && node1 && node2) {
            Circuit* c = static_cast<Circuit*>(circuit);
            Node* n1 = c->findOrCreateNode(node1);
            Node* n2 = c->findOrCreateNode(node2);
            c->voltageSources.emplace_back();
            VoltageSource& vs = c->voltageSources.back();
            vs.name = name;
            vs.node1 = n1;
            vs.node2 = n2;
            vs.value = voltage;
        }
    }
    
    // --- NEW: Implementation for AddACVoltageSource ---
    void AddACVoltageSource(void* circuit, const char* name, const char* node1, const char* node2, double magnitude, double phase) {
        // This function is a stub. You will need to implement the logic in your Circuit class
        // to handle ACVoltageSource components.
    }

    void SetGroundNode(void* circuit, const char* nodeName) {
        if (circuit && nodeName) {
            Circuit* c = static_cast<Circuit*>(circuit);
            Node* node = c->findOrCreateNode(nodeName);
            node->setGround(true);
        }
    }

    bool RunDCAnalysis(void* circuit) {
        if (circuit) {
            dcAnalysis(*static_cast<Circuit*>(circuit));
            return true;
        }
        return false;
    }

    bool RunTransientAnalysis(void* circuit, double stepTime, double stopTime) {
        if (circuit) {
            transientAnalysis(*static_cast<Circuit*>(circuit), stepTime, stopTime);
            return true;
        }
        return false;
    }

    // --- NEW: Stub implementation for RunACAnalysis ---
    bool RunACAnalysis(void* circuit, const char* sourceName, double startFreq, double stopFreq, int numPoints, const char* sweepType) {
        // This is a stub. The full AC analysis logic needs to be called here.
        return false; // Return false until implemented
    }

    double GetNodeVoltage(void* circuit, const char* nodeName) {
        if (circuit && nodeName) {
            Node* node = static_cast<Circuit*>(circuit)->findNode(nodeName);
            if (node) {
                return node->getVoltage();
            }
        }
        return 0.0;
    }

    // --- FIX: Implemented safe string marshalling by creating a comma-separated list ---
    int GetNodeNames(void* circuit, char* nodeNamesBuffer, int bufferSize) {
        if (!circuit || !nodeNamesBuffer || bufferSize == 0) {
            return 0;
        }

        Circuit* c = static_cast<Circuit*>(circuit);
        std::stringstream ss;
        bool first = true;
        for (const auto& node : c->nodes) {
            if (!first) {
                ss << ",";
            }
            ss << node->name;
            first = false;
        }
        
        std::string allNames = ss.str();
        if (allNames.length() < bufferSize) {
            strcpy_s(nodeNamesBuffer, bufferSize, allNames.c_str());
            return allNames.length();
        }
        
        // Buffer too small
        nodeNamesBuffer[0] = '\0';
        return 0;
    }

    int GetNodeVoltageHistory(void* circuit, const char* nodeName, double* timePoints, double* voltages, int maxCount) {
        if (circuit && nodeName && timePoints && voltages) {
            Node* node = static_cast<Circuit*>(circuit)->findNode(nodeName);
            if (node) {
                int count = 0;
                for (const auto& point : node->voltage_history) {
                    if (count >= maxCount) break;
                    timePoints[count] = point.first;
                    voltages[count] = point.second;
                    count++;
                }
                return count;
            }
        }
        return 0;
    }

    // --- NEW: Stub implementation for GetNodeSweepHistory ---
    int GetNodeSweepHistory(void* circuit, const char* nodeName, double* frequencies, double* magnitudes, int maxCount) {
        // This is a stub. You will need to retrieve data from your AC analysis results.
        return 0;
    }
}
