#include "CircuitIO.h" // Include its own header
#include "Node.h"      // Needs Node for checking node existence and setting ground
#include <iostream>    // For cout, cerr
#include <sstream>     // For istringstream
#include <fstream>     // For ifstream, ofstream
#include <stdexcept>   // For stod exceptions
#include <algorithm>   // For any_of (if you use it for string checks)

using namespace std;

bool command_handling(Circuit& circuit, const vector<string>& cmds, vector<vector<string>>& readCommands) {
    if (cmds.empty()) return true; // Skip empty lines

    if (cmds[0] == "end") {
        return false; // Signal to end circuit description input
    } else if (cmds[0] == "add") {
        if (cmds.size() < 2) { cerr << "Error: 'add' command needs more arguments." << endl; return true;}
        if (cmds[1] == "node") {
            if (cmds.size() < 3) { cerr << "Error: 'add node' needs a name." << endl; return true;}
            circuit.addNode(cmds[2]);
        } else if (cmds[1] == "resistor") { // add resistor <value> <name> <node1> <node2>
            if (cmds.size() < 6) { cerr << "Error: 'add resistor' needs value, name, node1, node2." << endl; return true;}
            Resistor newResistor;
            try { newResistor.resistance = stod(cmds[2]); } catch (const exception& e) { cerr << "Error: Invalid resistance value for " << cmds[3] << endl; return true; }
            newResistor.name = cmds[3];
            newResistor.node1 = circuit.findNode(cmds[4]);
            newResistor.node2 = circuit.findNode(cmds[5]);
            if (!newResistor.node1 || !newResistor.node2) { cerr << "Error: Node(s) not found for resistor " << newResistor.name << endl; return true; }
            circuit.resistors.push_back(newResistor);
        } else if (cmds[1] == "capacitor") { // add capacitor <value> <name> <node1> <node2>
            if (cmds.size() < 6) { cerr << "Error: 'add capacitor' needs value, name, node1, node2." << endl; return true;}
            Capacitor newCap;
            try { newCap.capacitance = stod(cmds[2]); } catch (const exception& e) { cerr << "Error: Invalid capacitance value for " << cmds[3] << endl; return true; }
            newCap.name = cmds[3];
            newCap.node1 = circuit.findNode(cmds[4]);
            newCap.node2 = circuit.findNode(cmds[5]);
            if (!newCap.node1 || !newCap.node2) { cerr << "Error: Node(s) not found for capacitor " << newCap.name << endl; return true; }
            circuit.capacitors.push_back(newCap);
        } else if (cmds[1] == "inductor") { // add inductor <value> <name> <node1> <node2>
            if (cmds.size() < 6) { cerr << "Error: 'add inductor' needs value, name, node1, node2." << endl; return true;}
            Inductor newInd;
            try { newInd.inductance = stod(cmds[2]); } catch (const exception& e) { cerr << "Error: Invalid inductance value for " << cmds[3] << endl; return true; }
            newInd.name = cmds[3];
            newInd.node1 = circuit.findNode(cmds[4]);
            newInd.node2 = circuit.findNode(cmds[5]);
            if (!newInd.node1 || !newInd.node2) { cerr << "Error: Node(s) not found for inductor " << newInd.name << endl; return true; }
            circuit.inductors.push_back(newInd);
        } else if (cmds[1] == "diode") { // add diode <name> <node1> <node2> [Is=<val>] [Vt=<val>] [n=<val>]
            if (cmds.size() < 5) { cerr << "Error: 'add diode' needs name, node1, node2." << endl; return true;}
            Diode newDiode;
            newDiode.name = cmds[2];
            newDiode.node1 = circuit.findNode(cmds[3]);
            newDiode.node2 = circuit.findNode(cmds[4]);
            if (!newDiode.node1 || !newDiode.node2) { cerr << "Error: Node(s) not found for diode " << newDiode.name << endl; return true; }
            // Optional parameters for diode
            for (size_t i = 5; i < cmds.size(); ++i) {
                string param = cmds[i];
                size_t eqPos = param.find('=');
                if (eqPos != string::npos) {
                    string key = param.substr(0, eqPos);
                    string valStr = param.substr(eqPos + 1);
                    try {
                        double val = stod(valStr);
                        if (key == "Is") newDiode.Is = val;
                        else if (key == "Vt") newDiode.Vt = val;
                        else if (key == "n") newDiode.n = val;
                    } catch (const exception& e) {
                        cerr << "Warning: Invalid value for diode parameter " << key << endl;
                    }
                }
            }
            circuit.diodes.push_back(newDiode);
        } else if (cmds[1] == "voltage" && cmds.size() > 2 && cmds[2] == "source") { // add voltage source <value> <node1_name> <node2_name> (VIN is main, this could be for others if generalized)
            if (cmds.size() < 6) { cerr << "Error: 'add voltage source' needs value, name (VIN implicit), node1, node2." << endl; return true;}
            // Assuming this command configures the main VIN source
            try { circuit.VIN.value = stod(cmds[3]); } catch (const exception& e) { cerr << "Error: Invalid voltage for VIN." << endl; return true; }
            circuit.VIN.node1 = circuit.findNode(cmds[4]); // Positive terminal
            circuit.VIN.node2 = circuit.findNode(cmds[5]); // Negative terminal
            if (!circuit.VIN.node1 || !circuit.VIN.node2) { cerr << "Error: Node(s) not found for VIN." << endl; return true;}
        } else if (cmds[1] == "current" && cmds.size() > 2 && cmds[2] == "source") { // add current source <value> <name> <node_from> <node_to>
            if (cmds.size() < 7) { cerr << "Error: 'add current source' needs value, name, node_from, node_to." << endl; return true;}
            CurrentSource newCS;
            try { newCS.value = stod(cmds[3]); } catch (const exception& e) { cerr << "Error: Invalid current value for " << cmds[4] << endl; return true; }
            newCS.name = cmds[4];
            newCS.node2 = circuit.findNode(cmds[5]); // Current flows FROM node2
            newCS.node1 = circuit.findNode(cmds[6]); // Current flows TO node1
            if (!newCS.node1 || !newCS.node2) { cerr << "Error: Node(s) not found for current source " << newCS.name << endl; return true; }
            circuit.currentSources.push_back(newCS);
        }
        else if (cmds[1] == "ground") { // add ground <node_name>
            if (cmds.size() < 3) { cerr << "Error: 'add ground' needs a node name." << endl; return true;}
            circuit.groundNodeNames.push_back(cmds[2]);
            Node* gndNode = circuit.findNode(cmds[2]);
            if (gndNode) {
                gndNode->setGround(true);
            } else {
                cerr << "Error: Node " << cmds[2] << " not found for ground connection." << endl;
            }
        } else {
            cerr << "Warning: Unknown 'add' command type: " << cmds[1] << endl;
        }
    } else if (cmds[0] == "read") { // read node voltage <name> | read current <comp_name> | read voltage <comp_name>
        readCommands.push_back(cmds);
    } else {
        cerr << "Warning: Unknown command: " << cmds[0] << endl;
    }
    return true; // Continue processing commands
}

void handleErrors(const Circuit& circuit) {
    cout << "// handleErrors function called" << endl;
    if (circuit.groundNodeNames.empty() && !circuit.nodes.empty()) {
        cerr << "Error: No ground node detected in the circuit. Analysis may be unstable or incorrect." << endl;
    }
    for (const auto& res : circuit.resistors) {
        if (res.resistance <= 0) {
            cerr << "Error: Non-positive resistance value for resistor " << res.name << " (" << res.resistance << " Ohms)." << endl;
        }
        if (!res.node1 || !res.node2) {
            cerr << "Error: Resistor " << res.name << " has one or more null nodes." << endl;
        }
    }
    for (const auto& cap : circuit.capacitors) {
        if (cap.capacitance <= 0) {
            cerr << "Error: Non-positive capacitance value for capacitor " << cap.name << " (" << cap.capacitance << " F)." << endl;
        }
        if (!cap.node1 || !cap.node2) {
            cerr << "Error: Capacitor " << cap.name << " has one or more null nodes." << endl;
        }
    }
    for (const auto& ind : circuit.inductors) {
        if (ind.inductance <= 0) {
            cerr << "Error: Non-positive inductance value for inductor " << ind.name << " (" << ind.inductance << " H)." << endl;
        }
        if (!ind.node1 || !ind.node2) {
            cerr << "Error: Inductor " << ind.name << " has one or more null nodes." << endl;
        }
    }
    if (circuit.VIN.node1 && circuit.VIN.node2 && circuit.VIN.node1 == circuit.VIN.node2) {
        cerr << "Error: VIN source has both terminals connected to the same node." << endl;
    }
    for(const auto& cs : circuit.currentSources) {
        if (cs.node1 && cs.node2 && cs.node1 == cs.node2) {
            cerr << "Error: Current source " << cs.name << " has both terminals connected to the same node." << endl;
        }
    }
    // Check for floating nodes (nodes not connected to anything or only to other floating nodes) - more complex check.
}

Circuit readCircuitFromFile(const string& filename) {
    cout << "// readCircuitFromFile function called with filename: " << filename << " (Not Implemented)" << endl;
    Circuit circuit;
    // Implementation would involve:
    // 1. Opening and reading the file line by line.
    // 2. Parsing commands similar to how cin is parsed in main (perhaps calling command_handling).
    // ifstream inputFile(filename);
    // if (!inputFile.is_open()) {
    //    cerr << "Error: Could not open circuit file " << filename << endl;
    //    return circuit; // Return empty circuit
    // }
    // string line;
    // while (getline(inputFile, line)) { /* ... parse line ... */ }
    // inputFile.close();
    return circuit;
}

void saveResultsToFile(const Circuit& circuit, const string& filename) {
    cout << "// saveResultsToFile function called with filename: " << filename << " (Not Implemented)" << endl;
    // Implementation would involve:
    // 1. Opening the file for writing.
    // 2. Iterating through solved node voltages and component currents/voltages.
    // 3. Writing them to the file in a structured format.
    // ofstream outputFile(filename);
    // if (!outputFile.is_open()) {
    //    cerr << "Error: Could not open file " << filename << " for saving results." << endl;
    //    return;
    // }
    // /* ... write data ... */
    // outputFile.close();
}