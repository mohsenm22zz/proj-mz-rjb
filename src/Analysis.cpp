#include "Analysis.h"
#include "LinearSolver.h"
#include "Node.h"
#include <iostream>
#include <vector>
#include <iomanip>

using namespace std;

void result_from_vec(Circuit& circuit, const vector<double>& solvedVoltages, const vector<Node*>& nonGroundNodes);

void dcAnalysis(Circuit& circuit) {
    cout << "// Performing DC Analysis..." << endl;
    circuit.setDeltaT(1e12);
    vector<Node*> nonGroundNodes;
    for (auto* node : circuit.nodes) {
        if (!node->isGround) {
            nonGroundNodes.push_back(node);
        }
    }

    if (!nonGroundNodes.empty() || !circuit.voltageSources.empty() || !circuit.inductors.empty()) {
        circuit.set_MNA_A();
        circuit.set_MNA_RHS();
        vector<double> solved_solution = gaussianElimination(circuit.MNA_A, circuit.MNA_RHS);
        result_from_vec(circuit, solved_solution, nonGroundNodes);

        int num_nodes = nonGroundNodes.size();
        for(size_t i = 0; i < circuit.voltageSources.size(); ++i) {
            circuit.voltageSources[i].setCurrent(solved_solution[num_nodes + i]);
        }
        for(size_t i = 0; i < circuit.inductors.size(); ++i) {
            circuit.inductors[i].setInductorCurrent(solved_solution[num_nodes + circuit.voltageSources.size() + i]);
        }
    }
    cout << "// DC Analysis complete." << endl;
}

void transientAnalysis(Circuit& circuit, double t_step, double t_stop) {
    cout << "// Performing Transient Analysis..." << endl;
    circuit.clearComponentHistory();
    dcAnalysis(circuit);
    circuit.updateComponentStates();
    circuit.setDeltaT(t_step);

    vector<Node*> nonGroundNodes;
    for (auto* node : circuit.nodes) {
        if (!node->isGround) {
            nonGroundNodes.push_back(node);
        }
    }

    for (double t = 0; t <= t_stop; t += t_step) {
        for (auto* node : circuit.nodes) {
            if (!node->isGround) {
                node->addVoltageHistoryPoint(t, node->getVoltage());
            }
        }
        for (auto& vs : circuit.voltageSources) {
            vs.addCurrentHistoryPoint(t, vs.getCurrent());
        }

        circuit.set_MNA_A();
        circuit.set_MNA_RHS();
        if (circuit.MNA_A.empty()) break;

        vector<double> solved_solution = gaussianElimination(circuit.MNA_A, circuit.MNA_RHS);
        result_from_vec(circuit, solved_solution, nonGroundNodes);

        int num_nodes = nonGroundNodes.size();
        for(size_t i = 0; i < circuit.voltageSources.size(); ++i) {
            circuit.voltageSources[i].setCurrent(solved_solution[num_nodes + i]);
        }
        for(size_t i = 0; i < circuit.inductors.size(); ++i) {
            circuit.inductors[i].setInductorCurrent(solved_solution[num_nodes + circuit.voltageSources.size() + i]);
        }
        circuit.updateComponentStates();
    }
}

void result_from_vec(Circuit& circuit, const vector<double>& solvedVoltages, const vector<Node*>& nonGroundNodes) {
    if (solvedVoltages.size() < nonGroundNodes.size()) {
        cerr << "Error: Solution vector size mismatch." << endl;
        return;
    }
    for (size_t i = 0; i < nonGroundNodes.size(); ++i) {
        nonGroundNodes[i]->setVoltage(solvedVoltages[i]);
    }
}