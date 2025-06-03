#include "Analysis.h" // Include its own header
#include "LinearSolver.h" // dcAnalysis uses gaussianElimination
#include "Node.h" // Though Circuit.h includes it, sometimes needed for specific type uses
#include <iostream>   // For cout, cerr
#include <vector>     // For vector
#include <map>        // For map
#include <algorithm>  // For fill

using namespace std;

void dcAnalysis(Circuit& circuit) {
    cout << "// Performing DC Analysis..." << endl;

    // Set ground nodes explicitly
    for (const string& gndName : circuit.groundNodeNames) {
        Node* node = circuit.findNode(gndName);
        if (node) {
            node->setGround(true); // This also sets voltage to 0
        }
    }

    // Count non-ground nodes for MNA matrix size
    vector<Node*> nonGroundNodes;
    map<int, int> nodeNumToMatrixIndex; // Map original node number to MNA matrix index
    vector<int> matrixIndexToNodeNum;    // Map MNA matrix index back to original node number

    int currentMatrixIdx = 0;
    for (auto &node : circuit.nodes) {
        if (!node.isGround) {
            nonGroundNodes.push_back(&node);
            nodeNumToMatrixIndex[node.num] = currentMatrixIdx;
            matrixIndexToNodeNum.push_back(node.num);
            currentMatrixIdx++;
        }
    }

    int nodesToSolve = nonGroundNodes.size();
    if (nodesToSolve > 0) {
        vector<vector<double>> A(nodesToSolve, vector<double>(nodesToSolve, 0.0));
        vector<double> b(nodesToSolve, 0.0);

        // Fill MNA matrices
        // 1. Resistors
        for (auto &res : circuit.resistors) {
            if (!res.node1 || !res.node2 || res.resistance == 0) continue;
            double g = 1.0 / res.resistance;
            Node *n1 = res.node1;
            Node *n2 = res.node2;

            if (!n1->isGround && !n2->isGround) {
                int mx1 = nodeNumToMatrixIndex[n1->num];
                int mx2 = nodeNumToMatrixIndex[n2->num];
                A[mx1][mx1] += g;
                A[mx2][mx2] += g;
                A[mx1][mx2] -= g;
                A[mx2][mx1] -= g;
            } else if (n1->isGround && !n2->isGround) {
                int mx2 = nodeNumToMatrixIndex[n2->num];
                A[mx2][mx2] += g;
                // b[mx2] -= g * n1->getVoltage(); // n1->getVoltage() is 0
            } else if (!n1->isGround && n2->isGround) {
                int mx1 = nodeNumToMatrixIndex[n1->num];
                A[mx1][mx1] += g;
                // b[mx1] -= g * n2->getVoltage(); // n2->getVoltage() is 0
            }
        }

        // 2. Voltage Sources (Main VIN)
        // This handles VIN if one of its terminals is ground, or if it directly sets a node voltage.
        // A more general MNA would handle floating sources with an extra current variable.
        for (auto &vs : circuit.voltageSources) {
//            if (!vs.node1 || !vs.node2) continue;
//            Node *n_to = vs.node1;    // Current flows TO this node
//            Node *n_from = vs.node2; // Current flows FROM this node
//
//            if (!n_to->isGround) {
//                b[nodeNumToMatrixIndex[n_to->num]] += vs.value;
//            }
//            if (!n_from->isGround) {
//                b[nodeNumToMatrixIndex[n_from->num]] -= vs.value;
//            }
        }

        // 3. Current Sources
        for (auto &cs : circuit.currentSources) {
            if (!cs.node1 || !cs.node2) continue;
            Node *n_to = cs.node1;    // Current flows TO this node
            Node *n_from = cs.node2; // Current flows FROM this node

            if (!n_to->isGround) {
                b[nodeNumToMatrixIndex[n_to->num]] += cs.value;
            }
            if (!n_from->isGround) {
                b[nodeNumToMatrixIndex[n_from->num]] -= cs.value;
            }
        }

        // 4. Capacitors (DC) - Open circuits, no contribution to A or b for DC steady state.
        // 5. Inductors (DC) - Short circuits. V_node1 = V_node2.
        for (auto& ind : circuit.inductors) {
            if (!ind.node1 || !ind.node2) continue;
            if (ind.node1->isGround && !ind.node2->isGround) { // Inductor shorts node2 to ground
                Node* n2 = ind.node2;
                int mx2 = nodeNumToMatrixIndex[n2->num];
                fill(A[mx2].begin(), A[mx2].end(), 0.0);
                A[mx2][mx2] = 1.0;
                b[mx2] = 0.0; // V_n2 = 0
            } else if (!ind.node1->isGround && ind.node2->isGround) { // Inductor shorts node1 to ground
                Node* n1 = ind.node1;
                int mx1 = nodeNumToMatrixIndex[n1->num];
                fill(A[mx1].begin(), A[mx1].end(), 0.0);
                A[mx1][mx1] = 1.0;
                b[mx1] = 0.0; // V_n1 = 0
            } else if (!ind.node1->isGround && !ind.node2->isGround) {
                cerr << "Warning: Inductor " << ind.name << " connects two non-ground nodes. Simplified DC MNA may not be accurate for V_L=0." << endl;
            }
        }

        // Solve the system Ax = b
        vector<double> solvedVoltages = gaussianElimination(A, b);

        if (solvedVoltages.size() == (size_t)nodesToSolve) {
            for (int i = 0; i < nodesToSolve; ++i) {
                Node *node = circuit.findNodeByNum(matrixIndexToNodeNum[i]);
                if (node) node->setVoltage(solvedVoltages[i]);
            }
        } else {
            cerr << "Error: Could not solve for node voltages. Matrix may be singular or ill-conditioned." << endl;
        }

        double i_vin_calc = 0.0;
        /// vs

    } else if (circuit.nodes.empty() && !circuit.groundNodeNames.empty()) {
        // Only ground nodes, all voltages are 0.
    } else if (circuit.nodes.empty()){
        // No nodes defined.
    } else {
        // All nodes are ground nodes.
        cout << "All nodes are ground. All voltages are 0." << endl;
    }
}

void transientAnalysis(Circuit& circuit, double tstep, double tstop) {
    cout << "// transientAnalysis function called (Not Implemented)" << endl;
    // Implementation would go here.
}