#include "Analysis.h"
#include "LinearSolver.h"
#include "Node.h"
#include <iostream>
#include <vector>
#include <iomanip>
#include <cmath> // For fabs

using namespace std;

void result_from_vec(Circuit& circuit, const vector<double>& solvedVoltages, const vector<Node*>& nonGroundNodes);

void dcAnalysis(Circuit& circuit) {
    cout << "// Performing DC Analysis..." << endl;
    circuit.setDeltaT(1e12); // For DC analysis, capacitors act as open, inductors as short

    vector<Node*> nonGroundNodes;
    for (auto* node : circuit.nodes) {
        if (!node->isGround) {
            nonGroundNodes.push_back(node);
        }
    }

    const int MAX_DIODE_ITERATIONS = 100;
    bool converged = false;
    int iteration_count = 0;

    // Initialize diode states (e.g., all off) - this needs to be done once before the loop
    for (auto& diode : circuit.diodes) {
        diode.setState(STATE_OFF);
    }

    do {
        converged = true;
        iteration_count++;

        // Store previous diode states
        vector<DiodeState> previous_diode_states;
        for (const auto& diode : circuit.diodes) {
            previous_diode_states.push_back(diode.getState());
        }

        circuit.set_MNA_A(); // MNA_A depends on diode states via Circuit::G()
        circuit.set_MNA_RHS(); // MNA_RHS depends on diode states via Circuit::J()

        if (circuit.MNA_A.empty() && circuit.MNA_RHS.empty() && circuit.nodes.empty() && circuit.voltageSources.empty() && circuit.inductors.empty()) {
            // Handle case of empty circuit or no solvable elements
            cout << "// No active components for MNA solution." << endl;
            break;
        }

        vector<double> solved_solution;
        try {
            solved_solution = gaussianElimination(circuit.MNA_A, circuit.MNA_RHS);
        } catch (const exception& e) {
            cerr << "Error during Gaussian Elimination: " << e.what() << endl;
            converged = false; // Mark as not converged to stop
            break;
        }

        result_from_vec(circuit, solved_solution, nonGroundNodes);

        // Update current sources for voltage sources and inductors based on solution
        int num_nodes = nonGroundNodes.size();
        for(size_t i = 0; i < circuit.voltageSources.size(); ++i) {
            circuit.voltageSources[i].setCurrent(solved_solution[num_nodes + i]);
        }
        for(size_t i = 0; i < circuit.inductors.size(); ++i) {
            circuit.inductors[i].setInductorCurrent(solved_solution[num_nodes + circuit.voltageSources.size() + i]);
        }

        // Update diode states based on new voltages and check for convergence
        for (size_t i = 0; i < circuit.diodes.size(); ++i) {
            Diode& current_diode = circuit.diodes[i];
            DiodeState old_state = previous_diode_states[i];

            double v_anode = current_diode.node1->getVoltage();
            double v_cathode = current_diode.node2->getVoltage();
            double v_diode = v_anode - v_cathode;

            DiodeState new_state = STATE_OFF;

            if (current_diode.getDiodeType() == NORMAL) {
                if (v_diode >= current_diode.getForwardVoltage()) {
                    new_state = STATE_FORWARD_ON;
                } else {
                    new_state = STATE_OFF;
                }
            } else if (current_diode.getDiodeType() == ZENER) {
                if (v_diode >= current_diode.getForwardVoltage()) {
                    new_state = STATE_FORWARD_ON;
                } else if (v_diode <= -current_diode.getZenerVoltage()) {
                    new_state = STATE_REVERSE_ON;
                } else {
                    new_state = STATE_OFF;
                }
            }
            current_diode.setState(new_state);

            if (new_state != old_state) {
                converged = false;
            }
        }

    } while (!converged && iteration_count < MAX_DIODE_ITERATIONS);

    if (!converged) {
        cerr << "Warning: DC Analysis did not converge after " << MAX_DIODE_ITERATIONS << " iterations for diodes." << endl;
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