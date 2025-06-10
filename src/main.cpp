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
    while (true) {
        Circuit circuit;
        string line;
        vector<vector<string>> analysisCommands;

        cout << "---=== [mz-rjb Circuit Simulator] ===---" << endl;
        cout << "Enter circuit commands. Type '.run' to simulate or 'exit' to quit." << endl;

        while (getline(cin, line)) {
            if (line == "exit") break;

            istringstream iss(line);
            vector<string> cmds;
            string cmd_part;
            while (iss >> cmd_part) {
                cmds.push_back(cmd_part);
            }

            if (!cmds.empty()) {
                if (!command_handling(circuit, cmds, analysisCommands)) break;
            }
        }

        if (line == "exit") break;

        clrscrr();
        cout << "---=== [Simulation Results] ===---" << endl;
        handleErrors(circuit);

        if (analysisCommands.empty()) {
            analysisCommands.push_back({".dc"});
        }

        for (const auto &cmd_parts: analysisCommands) {
            string analysisType = cmd_parts[0];
            transform(analysisType.begin(), analysisType.end(), analysisType.begin(), ::tolower);

            if (analysisType == ".dc") {
                dcAnalysis(circuit);
                cout << fixed << setprecision(4);
                cout << "\n// --- DC Analysis Results ---" << endl;
                for (auto* node : circuit.nodes) {
                    if(!node->isGround) {
                        cout << "  V(" << node->name << ") = " << node->getVoltage() << " V" << endl;
                    }
                }
                for (auto& vs : circuit.voltageSources) {
                    cout << "  I(" << vs.name << ") = " << vs.getCurrent() << " A" << endl;
                }
                cout << "// --------------------------" << endl;

            } else if (analysisType == ".tran") {
                if (cmd_parts.size() < 3) {
                    cerr << "Error: Format is '.tran <T_step> <T_stop>'" << endl;
                    continue;
                }
                try {
                    double t_step = stonum(cmd_parts[1]);
                    double t_stop = stonum(cmd_parts[2]);
                    transientAnalysis(circuit, t_step, t_stop);

                    cout << "\n// --- Stored History Example ---" << endl;
                    cout << fixed << setprecision(4);

                    for (const auto* node : circuit.nodes) {
                        if (!node->isGround) {
                            cout << "  History for Node " << node->name << ":" << endl;
                            for (const auto& point : node->voltage_history) {
                                cout << "    Time: " << setw(8) << left << point.first
                                     << " Voltage: " << point.second << " V" << endl;
                            }
                        }
                    }
                } catch (const exception &e) {
                    cerr << "Error: " << e.what() << endl;
                }
            }
        }
        cout << "\nSimulation finished. Circuit cleared." << endl;
    }
    return 0;
}