#include "Analysis.h"
#include "LinearSolver.h"
#include "Node.h"
#include <iostream>
#include <vector>
#include <iomanip>
#include <cmath>
#include <complex>

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

    const int MAX_DIODE_ITERATIONS = 100;
    bool converged = false;
    int iteration_count = 0;
    const double EPSILON_CURRENT = 1e-9;
    for (auto& diode : circuit.diodes) {
        diode.setState(STATE_OFF);
    }

    do {
        converged = true;
        iteration_count++;

        vector<DiodeState> previous_diode_states;
        for (const auto& diode : circuit.diodes) {
            previous_diode_states.push_back(diode.getState());
        }

        circuit.assignDiodeBranchIndices();
        circuit.set_MNA_A();
        circuit.set_MNA_RHS();

        if (circuit.MNA_A.empty() || circuit.MNA_A[0].empty() || circuit.MNA_RHS.empty() || circuit.MNA_A.size() != circuit.MNA_RHS.size()) {
            cout << "// No solvable MNA system for the current circuit state." << endl;
            break;
        }

        vector<double> solved_solution;
        try {
            solved_solution = gaussianElimination(circuit.MNA_A, circuit.MNA_RHS);
        } catch (const exception& e) {
            cerr << "Error during Gaussian Elimination: " << e.what() << endl;
            converged = false;
            break;
        }

        result_from_vec(circuit, solved_solution, nonGroundNodes);

        for (size_t i = 0; i < circuit.diodes.size(); ++i) {
            Diode& current_diode = circuit.diodes[i];
            DiodeState old_state = previous_diode_states[i];

            double v_anode = current_diode.node1->getVoltage();
            double v_cathode = current_diode.node2->getVoltage();
            double v_diode_across = v_anode - v_cathode;
            DiodeState new_state = old_state;

            if (current_diode.getDiodeType() == NORMAL) {
                if (old_state == STATE_OFF) {
                    if (v_diode_across >= current_diode.getForwardVoltage() - EPSILON_CURRENT) {
                        new_state = STATE_FORWARD_ON;
                    }
                } else if (old_state == STATE_FORWARD_ON) {
                    if (current_diode.getCurrent() < -EPSILON_CURRENT) {
                        new_state = STATE_OFF;
                    }
                }
            } else if (current_diode.getDiodeType() == ZENER) {
                if (old_state == STATE_OFF) {
                    if (v_diode_across >= current_diode.getForwardVoltage() - EPSILON_CURRENT) {
                        new_state = STATE_FORWARD_ON;
                    } else if (v_diode_across <= -current_diode.getZenerVoltage() + EPSILON_CURRENT) {
                        new_state = STATE_REVERSE_ON;
                    }
                } else if (old_state == STATE_FORWARD_ON) {
                    if (current_diode.getCurrent() < -EPSILON_CURRENT) {
                        new_state = STATE_OFF;
                    }
                } else if (old_state == STATE_REVERSE_ON) {
                    if (current_diode.getCurrent() > EPSILON_CURRENT) {
                        new_state = STATE_OFF;
                    }
                }
            }

            if (new_state != old_state) {
                converged = false;
                current_diode.setState(new_state);
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

    for (auto& cap : circuit.capacitors) {
        cap.prevVoltage = 0.0;
    }
    for (auto& ind : circuit.inductors) {
        ind.prevCurrent = 0.0;
    }

    circuit.setDeltaT(t_step);

    vector<Node*> nonGroundNodes;
    for (auto* node : circuit.nodes) {
        if (!node->isGround) {
            nonGroundNodes.push_back(node);
        }
    }

    // Add initial conditions at t=0
    for (auto* node : circuit.nodes) {
        if (!node->isGround) {
            node->addVoltageHistoryPoint(0.0, node->getVoltage());
        }
    }
    for (auto& vs : circuit.voltageSources) {
        vs.addCurrentHistoryPoint(0.0, vs.getCurrent());
    }

    for (double t = t_step; t <= t_stop; t += t_step) {
        const int MAX_DIODE_ITERATIONS = 100;
        const double EPSILON_CURRENT = 1e-9;
        bool converged = false;
        int iteration_count = 0;

        do {
            converged = true;
            iteration_count++;

            vector<DiodeState> previous_diode_states;
            for (const auto& diode : circuit.diodes) {
                previous_diode_states.push_back(diode.getState());
            }

            circuit.assignDiodeBranchIndices();
            circuit.set_MNA_A();
            circuit.set_MNA_RHS();

            vector<double> solved_solution;
            try {
                solved_solution = gaussianElimination(circuit.MNA_A, circuit.MNA_RHS);
            } catch (const exception& e) {
                cerr << "Error during Gaussian Elimination at t=" << t << ": " << e.what() << endl;
                break;
            }

            result_from_vec(circuit, solved_solution, nonGroundNodes);

            for (size_t i = 0; i < circuit.diodes.size(); ++i) {
                Diode& current_diode = circuit.diodes[i];
                DiodeState old_state = previous_diode_states[i];
                DiodeState new_state = old_state;

                double v_anode = current_diode.node1->getVoltage();
                double v_cathode = current_diode.node2->getVoltage();
                double v_diode_across = v_anode - v_cathode;

                if (current_diode.getDiodeType() == NORMAL) {
                    if (old_state == STATE_OFF) {
                        if (v_diode_across >= current_diode.getForwardVoltage() - EPSILON_CURRENT) {
                            new_state = STATE_FORWARD_ON;
                        }
                    } else if (old_state == STATE_FORWARD_ON) {
                        if (current_diode.getCurrent() < -EPSILON_CURRENT) {
                            new_state = STATE_OFF;
                        }
                    }
                } else if (current_diode.getDiodeType() == ZENER) {
                    if (old_state == STATE_OFF) {
                        if (v_diode_across >= current_diode.getForwardVoltage() - EPSILON_CURRENT) {
                            new_state = STATE_FORWARD_ON;
                        } else if (v_diode_across <= -current_diode.getZenerVoltage() + EPSILON_CURRENT) {
                            new_state = STATE_REVERSE_ON;
                        }
                    } else if (old_state == STATE_FORWARD_ON) {
                        if (current_diode.getCurrent() < -EPSILON_CURRENT) {
                            new_state = STATE_OFF;
                        }
                    } else if (old_state == STATE_REVERSE_ON) {
                        if (current_diode.getCurrent() > EPSILON_CURRENT) {
                            new_state = STATE_OFF;
                        }
                    }
                }

                if (new_state != old_state) {
                    converged = false;
                    current_diode.setState(new_state);
                }
            }
        } while (!converged && iteration_count < MAX_DIODE_ITERATIONS);

        if (!converged) {
            cerr << "Warning: Diode states did not converge at t=" << t << endl;
        }

        for (auto* node : circuit.nodes) {
            if (!node->isGround) {
                node->addVoltageHistoryPoint(t, node->getVoltage());
            }
        }
        for (auto& vs : circuit.voltageSources) {
            vs.addCurrentHistoryPoint(t, vs.getCurrent());
        }

        circuit.updateComponentStates();
    }
    cout << "// Transient Analysis complete." << endl;
}


void result_from_vec(Circuit& circuit, const vector<double>& solvedVoltages, const vector<Node*>& nonGroundNodes) {
    if (solvedVoltages.size() < nonGroundNodes.size()) {
        cerr << "Error: Solution vector size mismatch." << endl;
        return;
    }
    for (size_t i = 0; i < nonGroundNodes.size(); ++i) {
        nonGroundNodes[i]->setVoltage(solvedVoltages[i]);
    }

    int current_idx_offset = nonGroundNodes.size();

    for(size_t i = 0; i < circuit.voltageSources.size(); ++i) {
        circuit.voltageSources[i].setCurrent(solvedVoltages[current_idx_offset + i]);
    }
    current_idx_offset += circuit.voltageSources.size();

    for(size_t i = 0; i < circuit.inductors.size(); ++i) {
        circuit.inductors[i].setInductorCurrent(solvedVoltages[current_idx_offset + i]);
    }
    current_idx_offset += circuit.inductors.size();

    for (auto& diode : circuit.diodes) {
        if (diode.getState() == STATE_FORWARD_ON || diode.getState() == STATE_REVERSE_ON) {
            int diode_solution_idx = diode.getBranchIndex();
            if (diode_solution_idx != -1 && diode_solution_idx >= 0 && static_cast<size_t>(diode_solution_idx) < solvedVoltages.size()) {
                diode.setCurrent(solvedVoltages[diode_solution_idx]);
            } else {
                cerr << "Warning: Diode " << diode.name << " has invalid branch index or solution size mismatch. Cannot set current." << endl;
            }
        }
    }
}

void dcSweepAnalysis(Circuit& circuit, const string& sourceName, double start, double end, double step) {
    Component* sweepSourceComponent = nullptr;
    char sourceType = toupper(sourceName[0]);
    if (sourceType == 'V') sweepSourceComponent = circuit.findVoltageSource(sourceName);
    else if (sourceType == 'I') sweepSourceComponent = circuit.findCurrentSource(sourceName);

    if (!sweepSourceComponent) {
        cerr << "Error: Sweep source '" << sourceName << "' not found." << endl;
        return;
    }

    for (double value = start; value <= end; value += step) {
        if (sourceType == 'V') static_cast<VoltageSource*>(sweepSourceComponent)->value = value;
        else if (sourceType == 'I') static_cast<CurrentSource*>(sweepSourceComponent)->value = value;

        dcAnalysis(circuit);

        for (auto* node : circuit.nodes) {
            if (!node->isGround) {
                node->dc_sweep_history.push_back({value, node->getVoltage()});
            }
        }
        for (auto& vs : circuit.voltageSources) {
            vs.dc_sweep_current_history.push_back({value, vs.getCurrent()});
        }
    }
}

void acSweepAnalysis(Circuit& circuit, const std::string& sourceName, double start_freq, double stop_freq, int num_points, const std::string& sweep_type) {
    cout << "// Performing AC Sweep Analysis..." << endl;
    circuit.clearComponentHistory();
    ACVoltageSource* acSource = circuit.findACVoltageSource(sourceName);
    if (!acSource) {
        cerr << "Error: AC source '" << sourceName << "' not found for sweep." << endl;
        return;
    }

    vector<Node*> nonGroundNodes;
    for (auto* node : circuit.nodes) {
        if (!node->isGround) {
            nonGroundNodes.push_back(node);
        }
    }
    for (int i = 0; i < num_points; ++i) {
        double current_freq;
        // For now, we only implement linear sweep as an example
        // A full implementation would handle logarithmic sweeps (Decade/Octave)
        current_freq = start_freq + i * (stop_freq - start_freq) / (num_points - 1);
        
        if (current_freq == 0) continue; // Avoid division by zero for C/L

        // 1. Build the complex MNA matrices for the current frequency
        circuit.set_MNA_A(AnalysisType::AC_SWEEP, current_freq);
        circuit.set_MNA_RHS(AnalysisType::AC_SWEEP, current_freq);

        // 2. Solve the complex system of equations
        vector<complex<double>> solution;
        try {
            solution = gaussianElimination(circuit.MNA_A_Complex, circuit.MNA_RHS_Complex);
        } catch (const exception& e) {
            cerr << "Error during AC analysis at frequency " << current_freq << " Hz: " << e.what() << endl;
            continue; // Skip to the next frequency point
        }

        // 3. Store the results (magnitude of voltage/current)
        for (size_t j = 0; j < nonGroundNodes.size(); ++j) {
            double magnitude = abs(solution[j]);
            nonGroundNodes[j]->ac_sweep_history.push_back({current_freq, magnitude});
        }
        // You would also store current magnitudes for components here
    }

    cout << "// AC Sweep Analysis complete." << endl;
}

// --- NEW (Skeleton) ---
// Implementation for Phase Sweep would be similar
void phaseSweepAnalysis(Circuit& circuit, const std::string& sourceName, double base_freq, double start_phase, double stop_phase, int num_points) {
    cout << "// Performing Phase Sweep Analysis (Not fully implemented)..." << endl;
    // The logic would be very similar to acSweepAnalysis:
    // 1. Loop from start_phase to stop_phase.
    // 2. In each iteration, update the phase of the source component.
    // 3. Re-build the MNA_RHS_Complex matrix (MNA_A_Complex doesn't change if only phase is swept).
    // 4. Solve the system.
    // 5. Store the magnitude of the result vs. the phase.
}