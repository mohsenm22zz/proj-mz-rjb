#include "CircuitSimulatorInterface.h"
#include "Analysis.h"
#include <cstring>
#include <string>
#include <sstream>

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
    
    void AddACVoltageSource(void* circuit, const char* name, const char* node1, const char* node2, double magnitude, double phase) {
        // Stub
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

    bool RunACAnalysis(void* circuit, const char* sourceName, double startFreq, double stopFreq, int numPoints, const char* sweepType) {
        // Stub
        return false;
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

    int GetNodeNames(void* circuit, char* nodeNamesBuffer, int bufferSize) {
        if (!circuit || !nodeNamesBuffer || bufferSize == 0) return 0;
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
        if (allNames.length() < bufferSize) {
            strcpy_s(nodeNamesBuffer, bufferSize, allNames.c_str());
            return allNames.length() + 1;
        }
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

    int GetNodeSweepHistory(void* circuit, const char* nodeName, double* frequencies, double* magnitudes, int maxCount) {
        // Stub
        return 0;
    }

    int GetComponentCurrentHistory(void* circuit, const char* componentName, double* timePoints, double* currents, int maxCount) {
        if (circuit && componentName && timePoints && currents) {
            Circuit* c = static_cast<Circuit*>(circuit);
            VoltageSource* vs = c->findVoltageSource(componentName);
            if (vs) {
                int count = 0;
                for (const auto& point : vs->current_history) {
                    if (count >= maxCount) break;
                    timePoints[count] = point.first;
                    currents[count] = point.second;
                    count++;
                }
                return count;
            }
        }
        return 0;
    }

    // --- NEW: Implementation for getting all voltage source names ---
    int GetAllVoltageSourceNames(void* circuit, char* vsNamesBuffer, int bufferSize) {
        if (!circuit || !vsNamesBuffer || bufferSize == 0) return 0;
        Circuit* c = static_cast<Circuit*>(circuit);
        std::stringstream ss;
        bool first = true;
        for (const auto& vs : c->voltageSources) {
            if (!first) ss << ",";
            ss << vs.name;
            first = false;
        }
        std::string allNames = ss.str();
        if (allNames.length() < bufferSize) {
            strcpy_s(vsNamesBuffer, bufferSize, allNames.c_str());
            return allNames.length() + 1;
        }
        vsNamesBuffer[0] = '\0';
        return 0;
    }

    double GetVoltageSourceCurrent(void* circuit, const char* vsName) {
        if (circuit && vsName) {
            VoltageSource* vs = static_cast<Circuit*>(circuit)->findVoltageSource(vsName);
            if (vs) {
                return vs->getCurrent();
            }
        }
        return 0.0;
    }
}