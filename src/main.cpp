#include <iostream>
#include <iomanip>
#include <vector>
#include <sstream>
#include <string>

#include "Circuit.h"
#include "Analysis.h"
#include "CircuitIO.h"
#include "LinearSolver.h"
using namespace std;

int main() {
    //test_solver();
    Circuit circuit;
    string line;
    vector<vector<string>> commands;
    cout << fixed << setprecision(3);
    bool dc = true; //default
    while (getline(cin, line)) {
        istringstream iss(line);
        vector<string> cmds;
        string cmd;
        while (iss >> cmd) {
            cmds.push_back(cmd);
        }
        if (!command_handling(circuit, cmds, commands,dc)) {
            break;
        }
    }
    handleErrors(circuit);
    if (dc)
    dcAnalysis(circuit);
    else transientAnalysis(circuit);
    for (const auto &cmds : commands) {
        if (cmds.size() < 3) { cerr << "Error: 'read' command is incomplete." << endl; continue; }

        if (cmds[1] == "node" && cmds[2] == "voltage") {
            if (cmds.size() < 4) { cerr << "Error: 'read node voltage' needs node name." << endl; continue; }
            Node *n = circuit.findNode(cmds[3]);
            if (n) {
                cout << n->name << " voltage = " << n->getVoltage() << " V" << endl;
            } else {
                cerr << "Error: Node " << cmds[3] << " not found for reading voltage." << endl;
            }
        } else if (cmds[1] == "current") {
            if (cmds.size() < 3) { cerr << "Error: 'read current' needs component name." << endl; continue; }
            string compName = cmds[2];
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
        } else if (cmds[1] == "voltage") { // read voltage <component_name_or_VIN>
            if (cmds.size() < 3) { cerr << "Error: 'read voltage' needs component name." << endl; continue; }
            string compName = cmds[2];
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
            cerr << "Warning: Unknown 'read' command: " << cmds[1] << endl;
        }
    }

    return 0;
}