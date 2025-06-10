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
    Circuit temp;/// Original Circuit without Diodes
    for (Node *n: circuit.nodes){
        temp.nodes.push_back(n);
    }
    for (Resistor r: circuit.resistors){
        temp.resistors.push_back(r);
    }
    for (Capacitor c: circuit.capacitors){
        temp.capacitors.push_back(c);
    }
    for (Inductor l: circuit.inductors){
        temp.inductors.push_back(l);
    }
    for (VoltageSource v: circuit.voltageSources){
        temp.voltageSources.push_back(v);
    }
    for (CurrentSource c: circuit.currentSources){
        temp.currentSources.push_back(c);
    }
    for (string string1: circuit.groundNodeNames){
        temp.groundNodeNames.push_back(string1);
    }
    Circuit temp2 = temp;
    /// complete from here with adding 0 voltage and curren sources
    /// 2^n case
    /// then repeat for transient
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