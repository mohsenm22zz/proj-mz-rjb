#include <iostream>
#include <iomanip>
#include <vector>
#include <sstream>
#include <string>
#include <algorithm>
#include <cstdlib>
#include "Circuit.h"
#include "Analysis.h"
#include "CircuitIO.h"
#include "LinearSolver.h"

using namespace std;

double stonum(const string &s);

void clrscrr();

int main() {
    //test_solver();
    Circuit circuit;
    string line;
    vector<vector<string>> analysisCommands;

    cout << "---=== [mz-rjb Circuit Simulator] ===---" << endl;
    cout << "Enter circuit (.run for simulation):" << endl;
    while (true) {
        while (getline(cin, line)) {
            if (line == "exit") {
                break;
            }
            istringstream iss(line);
            vector<string> cmds;
            string cmd_part;
            while (iss >> cmd_part) {
                cmds.push_back(cmd_part);
            }
            if (!cmds.empty()) {
                if (!command_handling(circuit, cmds, analysisCommands))
                    break;
            }
        }
        if (line == "exit") {
            break;
        }
        clrscrr();
        cout << "---=== [mz-rjb Circuit Simulator] ===---" << endl;
        handleErrors(circuit);
        for (const auto &cmd_parts: analysisCommands) {
            if (cmd_parts.empty()) continue;
            string analysisType = cmd_parts[0];
            transform(analysisType.begin(), analysisType.end(), analysisType.begin(), ::tolower);
            if (analysisType == ".dc") {
                dcAnalysis(circuit);
                for (auto &node: circuit.nodes) {
                    cout << "Node " << node.name << " voltage = " << node.getVoltage() << " V" << endl;
                }
            } else if (analysisType == ".tran") {
                if (cmd_parts.size() < 3) {
                    cerr << "Error: Format: .tran <Tstep> <Tstop>" << endl;
                    continue;
                }
                try {
                    double t_step = stonum(cmd_parts[1]);
                    double t_stop = stonum(cmd_parts[2]);
                    transientAnalysis(circuit, t_step, t_stop);
                } catch (const exception &e) {
                    cerr << "Error parsing transient analysis parameters: " << e.what() << endl;
                }
            }
        }
        if (analysisCommands.empty()) {
            dcAnalysis(circuit);
            for (auto &node: circuit.nodes) {
                cout << "Node " << node.name << " voltage = " << node.getVoltage() << " V" << endl;
            }
        }
    }
    return 0;
}
