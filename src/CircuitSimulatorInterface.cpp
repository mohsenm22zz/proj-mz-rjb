#include "CircuitSimulatorInterface.h"
#include "Analysis.h"
#include <cstring>
#include <algorithm>

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
            // Create resistor and set properties
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
            // Create voltage source and set properties
            c->voltageSources.emplace_back();
            VoltageSource& voltageSource = c->voltageSources.back();
            voltageSource.name = name;
            voltageSource.node1 = n1;
            voltageSource.node2 = n2;
            voltageSource.value = voltage;
        }
    }

    void SetGroundNode(void* circuit, const char* nodeName) {
        if (circuit && nodeName) {
            Circuit* c = static_cast<Circuit*>(circuit);
            Node* node = c->findNode(nodeName);
            if (node) {
                node->setGround(true);
            }
            else {
                node = c->findOrCreateNode(nodeName);
                node->setGround(true);
            }
        }
    }

    bool RunDCAnalysis(void* circuit) {
        if (circuit) {
            Circuit* c = static_cast<Circuit*>(circuit);
            // Simple DC analysis implementation
            dcAnalysis(*c);
            return true; // Assuming success for now
        }
        return false;
    }

    bool RunTransientAnalysis(void* circuit, double stepTime, double stopTime) {
        if (circuit) {
            Circuit* c = static_cast<Circuit*>(circuit);
            transientAnalysis(*c, stepTime, stopTime);
            return true; // Assuming success for now
        }
        return false;
    }

    double GetNodeVoltage(void* circuit, const char* nodeName) {
        if (circuit && nodeName) {
            Circuit* c = static_cast<Circuit*>(circuit);
            Node* node = c->findNode(nodeName);
            if (node) {
                return node->getVoltage();
            }
        }
        return 0.0;
    }

    int GetNodeNames(void* circuit, const char** nodeNames, int maxCount) {
        if (circuit && nodeNames) {
            Circuit* c = static_cast<Circuit*>(circuit);
            int count = 0;
            for (const auto& node : c->nodes) {
                if (count >= maxCount) break;
                // Note: This is a simplified approach. In a real implementation,
                // you would need to manage string memory more carefully.
                nodeNames[count] = node->name.c_str();
                count++;
            }
            return count;
        }
        return 0;
    }

    int GetNodeVoltageHistory(void* circuit, const char* nodeName, double* timePoints, double* voltages, int maxCount) {
        if (circuit && nodeName && timePoints && voltages) {
            Circuit* c = static_cast<Circuit*>(circuit);
            Node* node = c->findNode(nodeName);
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
}