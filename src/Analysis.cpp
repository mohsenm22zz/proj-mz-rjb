#include "Analysis.h"
#include "LinearSolver.h"
#include "Node.h"
#include <iostream>
#include <vector>
#include <iomanip>
using namespace std;

void result_from_vec(Circuit& circuit, const vector<double>& solvedVoltages, const vector<Node*>& nonGroundNodes) {
    if (solvedVoltages.size() < nonGroundNodes.size()) {
        cerr << "Error: Solution vector size mismatch." << endl;
        return;
    }
    for (size_t i = 0; i < nonGroundNodes.size(); ++i) {
        nonGroundNodes[i]->setVoltage(solvedVoltages[i]);
    }
}

void dcAnalysis(Circuit& circuit) {
    cout << "// Performing DC Analysis..." << endl;
    circuit.setDeltaT(1e12);
    vector<Node*> nonGroundNodes;
    for (auto& node : circuit.nodes) {
        if (!node.isGround) {
            nonGroundNodes.push_back(&node);
        }
    }
    int nodesToSolve = nonGroundNodes.size();
    if (nodesToSolve > 0) {
        circuit.set_MNA_A();
        circuit.set_MNA_RHS();
        for(size_t i = 0; i < circuit.inductors.size(); ++i) {
            int m_idx = circuit.countNonGroundNodes() + circuit.voltageSources.size() + i;
            circuit.MNA_A[m_idx][m_idx] = 0; // In DC, inductor has no impedance itself
        }
        vector<double> solved_solution = gaussianElimination(circuit.MNA_A, circuit.MNA_RHS);
        result_from_vec(circuit, solved_solution, nonGroundNodes);
    }
    cout << "// DC Analysis complete." << endl;
}

void transientAnalysis(Circuit& circuit, double t_step, double t_stop) {
    cout << "// Performing Transient Analysis..." << endl;

    dcAnalysis(circuit);
    circuit.updateComponentStates();
    circuit.setDeltaT(t_step/10);



    vector<string> headers;
    headers.push_back("time");
    for (const auto& node : circuit.nodes) {
        if (!node.isGround) {
            headers.push_back("V(" + node.name + ")");
        }
    }
    for (const auto& comp : circuit.resistors) { headers.push_back("I(" + comp.name + ")"); }
    for (const auto& comp : circuit.capacitors) { headers.push_back("I(" + comp.name + ")"); }
    for (const auto& comp : circuit.inductors) { headers.push_back("I(" + comp.name + ")"); }
    for (const auto& comp : circuit.voltageSources) { headers.push_back("I(" + comp.name + ")"); }


    vector<vector<double>> results;

    vector<Node*> nonGroundNodes;
    for (auto& node : circuit.nodes) {
        if (!node.isGround) {
            nonGroundNodes.push_back(&node);
        }
    }
    int num_nodes = nonGroundNodes.size();
    int num_vs = circuit.voltageSources.size();



    for (double t = 0; t <= t_stop; t += t_step) {
        vector<double> current_step_results;
        current_step_results.push_back(t);
        for (const auto& node_ptr : nonGroundNodes) {
            current_step_results.push_back(node_ptr->getVoltage());
        }
        for (auto& comp : circuit.resistors) { current_step_results.push_back(comp.getCurrent()); }
        for (auto& comp : circuit.capacitors) { current_step_results.push_back(comp.getCurrent()); }
        for (auto& comp : circuit.inductors) { current_step_results.push_back(comp.getCurrent()); }
        for (auto& comp : circuit.voltageSources) { current_step_results.push_back(comp.getCurrent()); }
        results.push_back(current_step_results);

        circuit.set_MNA_A();
        circuit.set_MNA_RHS();

        if (circuit.MNA_A.empty()) {
            cout << "Empty Circuit" << endl;
            break;
        }

        vector<double> solved_solution = gaussianElimination(circuit.MNA_A, circuit.MNA_RHS);
        result_from_vec(circuit, solved_solution, nonGroundNodes);
        for(size_t i = 0; i < circuit.voltageSources.size(); ++i) {
            circuit.voltageSources[i].setCurrent(solved_solution[num_nodes + i]);
        }
        for(size_t i = 0; i < circuit.inductors.size(); ++i) {
            circuit.inductors[i].setInductorCurrent(solved_solution[num_nodes + num_vs + i]);
        }
        for(auto& cap : circuit.capacitors) {
            double v_new = (cap.node1->getVoltage() - cap.node2->getVoltage());
            double i_c = (cap.capacitance / t_step) * (v_new - cap.prevVoltage);
            cap.setCurrent(i_c);
        }
        circuit.updateComponentStates();
    }


    cout << "\n// Transient Analysis Results:" << endl;
    for (const auto& header : headers) {
        cout << setw(12) << left << header;
    }
    cout << endl;
    cout << fixed << setprecision(4);
    for (const auto& row : results) {
        for (const auto& val : row) {
            cout << setw(12) << left << val;
        }
        cout << endl;
    }
}
