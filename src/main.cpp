#include <iostream>
#include <iomanip>
#include <vector>
#include <sstream>
#include <string>

#include "Circuit.h"
#include "Analysis.h"
#include "CircuitIO.h"

using namespace std;

int main() {
    Circuit circuit;
    string line;
    vector<vector<string>> readCommands; // Commands to execute after solving

    cout << fixed << setprecision(3); // Set precision for output

    // --- Input Phase ---
    // Reads circuit description from standard input using the CircuitIO function
    while (getline(cin, line)) {
        istringstream iss(line);
        vector<string> cmds;
        string cmd_part;
        while (iss >> cmd_part) {
            cmds.push_back(cmd_part);
        }

        if (!command_handling(circuit, cmds, readCommands)) {
            break; // 'end' command received
        }
    }

    // --- Error Handling ---
    handleErrors(circuit); // Call the error handling function

    // --- DC Analysis ---
    dcAnalysis(circuit); // Call the DC analysis function

    // --- Output Phase ---
    // Iterate through stored 'read' commands and output results
    for (const auto &cmd_parts : readCommands) {
        if (cmd_parts.size() < 3) { cerr << "Error: 'read' command is incomplete." << endl; continue; }

        if (cmd_parts[1] == "node" && cmd_parts[2] == "voltage") {
            if (cmd_parts.size() < 4) { cerr << "Error: 'read node voltage' needs node name." << endl; continue; }
            Node *n = circuit.findNode(cmd_parts[3]);
            if (n) {
                cout << n->name << " voltage = " << n->getVoltage() << " V" << endl;
            } else {
                cerr << "Error: Node " << cmd_parts[3] << " not found for reading voltage." << endl;
            }
        } else if (cmd_parts[1] == "current") { // read current <component_name_or_VIN>
            if (cmd_parts.size() < 3) { cerr << "Error: 'read current' needs component name." << endl; continue; }
            string compName = cmd_parts[2];
            if (compName == "VIN") {

            } else if (Resistor* r = circuit.findResistor(compName)) {
                cout << r->name << " current = " << fabs(r->getCurrent()) << " A" << endl;
            } else if (Capacitor* c = circuit.findCapacitor(compName)) {
                cout << c->name << " current (DC) = " << fabs(c->getCurrent()) << " A" << endl; // Should be 0 for DC
            } else if (Inductor* i = circuit.findInductor(compName)) {
                cout << i->name << " current (DC) = " << fabs(i->getCurrent()) << " A" << endl;
            } else if (Diode* d = circuit.findDiode(compName)) {
                cout << d->name << " current = " << d->getCurrent() << " A" << endl;
            } else if (CurrentSource* cs = circuit.findCurrentSource(compName)) {
                cout << cs->name << " current = " << cs->getCurrent() << " A" << endl;
            }
            else {
                cerr << "Error: Component " << compName << " not found for reading current." << endl;
            }
        } else if (cmd_parts[1] == "voltage") { // read voltage <component_name_or_VIN>
            if (cmd_parts.size() < 3) { cerr << "Error: 'read voltage' needs component name." << endl; continue; }
            string compName = cmd_parts[2];
            if (compName == "VIN") {

            } else if (Resistor* r = circuit.findResistor(compName)) {
                cout << r->name << " voltage = " << r->getVoltage() << " V" << endl;
            } else if (Capacitor* c = circuit.findCapacitor(compName)) {
                cout << c->name << " voltage (DC) = " << c->getVoltage() << " V" << endl;
            } else if (Inductor* i = circuit.findInductor(compName)) {
                cout << i->name << " voltage (DC) = " << i->getVoltage() << " V" << endl; // Should be 0 for DC
            } else if (Diode* d = circuit.findDiode(compName)) {
                cout << d->name << " voltage = " << d->getVoltage() << " V" << endl;
            } else if (CurrentSource* cs = circuit.findCurrentSource(compName)) {
                cout << cs->name << " voltage = " << cs->getVoltage() << " V" << endl;
            }
            else {
                cerr << "Error: Component " << compName << " not found for reading voltage." << endl;
            }
        } else {
            cerr << "Warning: Unknown 'read' command: " << cmd_parts[1] << endl;
        }
    }

    return 0;
}