#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>
#include <cmath>
#include <map>
#include <algorithm> // For std::any_of, std::stod for older C++ versions if needed

// It's good practice to use std:: prefix or import specific names
// For brevity in this example, 'using namespace std;' is kept from original
using namespace std;

// Forward declarations
class Node;
class Component;
class Resistor;
class Capacitor; // Corrected spelling and to be defined
class Inductor;  // To be defined
class Diode;     // To be defined
class VoltageSource;
class CurrentSource; // To be defined
class Circuit;

// Node Class
class Node {
public:
    string name;
    int num; // Unique numerical identifier for matrix operations
    static int nextNum;
    double voltage;
    bool isGround; // Flag to indicate if the node is a ground node

    Node() : name(""), num(nextNum++), voltage(0.0), isGround(false) {}

    double getVoltage() const { // const correct
        if (isGround) return 0.0;
        return voltage;
    }

    void setVoltage(double v) {
        if (isGround) {
            voltage = 0.0; // Ground node voltage is always 0
        } else {
            voltage = v;
        }
    }

    void setGround(bool ground_status) {
        isGround = ground_status;
        if (isGround) {
            voltage = 0.0;
        }
    }
};

int Node::nextNum = 0; // Initialize static member

// Component Base Class
class Component {
public:
    string name;
    Node *node1;
    Node *node2;

    Component() : name(""), node1(nullptr), node2(nullptr) {}

    // Pure virtual functions for getting current and voltage across the component
    virtual double getCurrent() = 0;
    virtual double getVoltage() = 0;

    // Virtual function for setting current (primarily for sources)
    virtual void setCurrent(double c) {};

    virtual ~Component() {}; // Virtual destructor for proper cleanup
};

// Resistor Class
class Resistor : public Component {
public:
    double resistance;

    Resistor() : resistance(0.0) {}

    double getCurrent() override {
        if (resistance == 0) return HUGE_VAL; // Avoid division by zero, though ideally resistance shouldn't be 0
        if (!node1 || !node2) return 0.0; // Should not happen if circuit is well-formed
        return (node1->getVoltage() - node2->getVoltage()) / resistance;
    }

    double getVoltage() override {
        if (!node1 || !node2) return 0.0;
        return fabs(node1->getVoltage() - node2->getVoltage());
    }
};

// Capacitor Class (from second snippet)
class Capacitor : public Component {
public:
    double capacitance;
    // Voltage and current members here might represent instantaneous values or values at a specific time step
    // For DC analysis, these are less relevant than its open-circuit behavior.
    // prevVoltage is for transient analysis.
    double prevVoltage; // Voltage at the previous time step for transient analysis

    Capacitor() : capacitance(0.0), prevVoltage(0.0) {}

    // getCurrent for a capacitor is C * dV/dt.
    // For DC analysis, current is 0.
    // The provided getCurrent seems to be for a discrete time step (Euler).
    double getCurrent() override {
        // This implementation is for transient analysis (e.g., Backward Euler with dt=1 or similar)
        // For DC analysis, a capacitor's current is 0.
        // return capacitance * ((node1->getVoltage() - node2->getVoltage()) - prevVoltage); // Assuming dt=1 for simplicity if this is from a time step
        return 0.0; // In DC, capacitor is an open circuit, so current is 0.
    }

    double getVoltage() override {
        if (!node1 || !node2) return 0.0;
        return fabs(node1->getVoltage() - node2->getVoltage());
    }

    // update method for transient analysis
    void update(double dt) { // dt is the time step
        if (!node1 || !node2) return;
        // This method would typically be used in transient analysis to update states.
        // For example, prevVoltage could be updated here.
        // prevVoltage = node1->getVoltage() - node2->getVoltage();
        // Or, if voltage is a state variable for the capacitor:
        // voltage += (1/capacitance) * current_through_capacitor * dt;
        // The provided 'prevVoltage' update seems to store the voltage difference for the next step's dV calculation.
        prevVoltage = node1->getVoltage() - node2->getVoltage();
    }
};

// Inductor Class (from second snippet)
class Inductor : public Component {
public:
    double inductance;
    double current; // Current through the inductor (state variable for transient)
    double prevCurrent; // Current at the previous time step for transient analysis

    Inductor() : inductance(0.0), current(0.0), prevCurrent(0.0) {}

    // getCurrent for an inductor is its state variable in many formulations.
    double getCurrent() override {
        // For DC analysis, current can be non-zero if there's a DC path.
        // Voltage across it will be 0.
        // This 'current' member would be solved for in a transient analysis.
        // For DC, it's determined by the rest of the circuit.
        // If we need to calculate it based on node voltages after MNA:
        // This is complex as V = L * dI/dt, so in DC, V=0, I can be anything.
        // For now, returning the stored 'current' which might be set by analysis.
        return current;
    }

    // getVoltage for an inductor is L * dI/dt.
    // For DC analysis, voltage is 0.
    double getVoltage() override {
        // In DC, an ideal inductor is a short circuit, so voltage across it is 0.
        // The provided implementation returns the voltage difference of its nodes,
        // which should be 0 after DC analysis if it's treated as a short.
        // return fabs(node1->getVoltage() - node2->getVoltage());
        return 0.0; // In DC, inductor is a short circuit.
    }

    // update method for transient analysis
    void update(double dt) { // dt is the time step
        if (!node1 || !node2) return;
        // This method would typically be used in transient analysis.
        // For example, current could be updated here:
        // current += (1/inductance) * (node1->getVoltage() - node2->getVoltage()) * dt;
        // The provided 'prevCurrent' update:
        prevCurrent = current;
    }
    // Method to explicitly set current, e.g., after MNA solves for it.
    void setInductorCurrent(double c) {
        this->current = c;
    }
};

// Diode Class (from second snippet)
class Diode : public Component {
public:
    double Is; // Reverse saturation current
    double Vt; // Thermal voltage (kT/q)
    double n;  // Ideality factor

    Diode() : Is(1e-12), Vt(0.026), n(1.0) {} // Default typical values

    double getCurrent() override {
        if (!node1 || !node2) return 0.0;
        double Vd = node1->getVoltage() - node2->getVoltage(); // Voltage across diode
        // Shockley diode equation: I = Is * (exp(Vd / (n * Vt)) - 1)
        // Add safeguards for large Vd to prevent overflow with exp()
        double exponent_val = Vd / (n * Vt);
        if (exponent_val > 70) { // exp(70) is already very large
            return Is * (exp(70.0) -1.0) ; // Cap current to avoid overflow
        } else if (exponent_val < -70) { // For large negative Vd, I approaches -Is
            return -Is;
        }
        return Is * (exp(exponent_val) - 1.0);
    }

    double getVoltage() override {
        if (!node1 || !node2) return 0.0;
        return fabs(node1->getVoltage() - node2->getVoltage());
    }
};

// VoltageSource Class
class VoltageSource : public Component {
public:
    double value;    // Voltage value of the source
    double current; // Current flowing through the source (to be calculated)

    VoltageSource() : value(0.0), current(0.0) {}

    // Current through an ideal voltage source is determined by the circuit.
    // It's not defined by the source itself.
    double getCurrent() override { return current; }

    void setCurrent(double c) override { current = c; }

    // Voltage across an ideal voltage source is its defined value.
    double getVoltage() override {
        return value; // The problem asks for fabs(value), but source can be negative
        // For reporting, fabs might be used, but actual value is 'value'
    }
};

// CurrentSource Class (from second snippet)
class CurrentSource : public Component {
public:
    double value; // Current value of the source
    // Voltage across a current source is determined by the circuit.

    CurrentSource() : value(0.0) {}

    // Current through an ideal current source is its defined value.
    double getCurrent() override { return value; }

    // Voltage across an ideal current source is determined by the circuit.
    double getVoltage() override {
        if (!node1 || !node2) return 0.0;
        return fabs(node1->getVoltage() - node2->getVoltage());
    }
};


// Circuit Class
class Circuit {
public:
    vector<Node> nodes;
    vector<Resistor> resistors;
    vector<Capacitor> capacitors;
    vector<Inductor> inductors;
    vector<Diode> diodes;
    VoltageSource VIN; // Assuming one primary DC voltage source for now as in original
    vector<CurrentSource> currentSources;
    vector<string> groundNodeNames; // Names of nodes designated as ground

    Circuit() {
        VIN.name = "VIN"; // Default name for the main voltage source
    }

    Node* findNode(const string& find_from_name) {
        for (auto &node : nodes) {
            if (node.name == find_from_name) return &node;
        }
        // If not found, create a new node? Or handle error?
        // For now, assuming nodes are added explicitly first.
        return nullptr;
    }

    Node* findNodeByNum(int num_to_find) {
        for (auto &node : nodes) {
            if (node.num == num_to_find) return &node;
        }
        return nullptr;
    }

    Resistor* findResistor(const string& find_from_name) {
        for (auto &res : resistors) {
            if (res.name == find_from_name) return &res;
        }
        return nullptr;
    }
    Capacitor* findCapacitor(const string& find_from_name) {
        for (auto &cap : capacitors) {
            if (cap.name == find_from_name) return &cap;
        }
        return nullptr;
    }
    Inductor* findInductor(const string& find_from_name) {
        for (auto &ind : inductors) {
            if (ind.name == find_from_name) return &ind;
        }
        return nullptr;
    }
    Diode* findDiode(const string& find_from_name) {
        for (auto &dio : diodes) {
            if (dio.name == find_from_name) return &dio;
        }
        return nullptr;
    }
    CurrentSource* findCurrentSource(const string& find_from_name) {
        for (auto &cs : currentSources) {
            if (cs.name == find_from_name) return &cs;
        }
        return nullptr;
    }


    // Stubs for MNA matrix construction if moved into Circuit class
    // vector<vector<double>> MNA_A() {};
    // vector<double> MNA_b() {}; // Renamed from MNA_v
    // vector<double> MNA_solve() {}; // Renamed from MNA_x

    void addNode(const string& name) {
        if (!findNode(name)) {
            Node newNode;
            newNode.name = name;
            nodes.push_back(newNode);
        }
    }
};

// Function Prototypes for Analysis and Utilities
vector<double> gaussianElimination(vector<vector<double>> A, vector<double> b);
void transientAnalysis(Circuit& circuit, double tstep, double tstop);
void dcAnalysis(Circuit& circuit); // More comprehensive DC analysis
void handleErrors(const Circuit& circuit);
Circuit readCircuitFromFile(const string& filename);
void saveResultsToFile(const Circuit& circuit, const string& filename);

// Function to handle circuit commands
bool command_handling(Circuit& circuit, const vector<string>& cmds, vector<vector<string>>& readCommands) {
    if (cmds.empty()) return true; // Skip empty lines

    if (cmds[0] == "end") {
        return false; // Signal to end circuit description input
    } else if (cmds[0] == "add") {
        if (cmds.size() < 2) { cerr << "Error: 'add' command needs more arguments." << endl; return true;}
        if (cmds[1] == "node") {
            if (cmds.size() < 3) { cerr << "Error: 'add node' needs a name." << endl; return true;}
            circuit.addNode(cmds[2]);
        } else if (cmds[1] == "resistor") { // add resistor <value> <name> <node1> <node2>
            if (cmds.size() < 6) { cerr << "Error: 'add resistor' needs value, name, node1, node2." << endl; return true;}
            Resistor newResistor;
            try { newResistor.resistance = stod(cmds[2]); } catch (const std::exception& e) { cerr << "Error: Invalid resistance value for " << cmds[3] << endl; return true; }
            newResistor.name = cmds[3];
            newResistor.node1 = circuit.findNode(cmds[4]);
            newResistor.node2 = circuit.findNode(cmds[5]);
            if (!newResistor.node1 || !newResistor.node2) { cerr << "Error: Node(s) not found for resistor " << newResistor.name << endl; return true; }
            circuit.resistors.push_back(newResistor);
        } else if (cmds[1] == "capacitor") { // add capacitor <value> <name> <node1> <node2>
            if (cmds.size() < 6) { cerr << "Error: 'add capacitor' needs value, name, node1, node2." << endl; return true;}
            Capacitor newCap;
            try { newCap.capacitance = stod(cmds[2]); } catch (const std::exception& e) { cerr << "Error: Invalid capacitance value for " << cmds[3] << endl; return true; }
            newCap.name = cmds[3];
            newCap.node1 = circuit.findNode(cmds[4]);
            newCap.node2 = circuit.findNode(cmds[5]);
            if (!newCap.node1 || !newCap.node2) { cerr << "Error: Node(s) not found for capacitor " << newCap.name << endl; return true; }
            circuit.capacitors.push_back(newCap);
        } else if (cmds[1] == "inductor") { // add inductor <value> <name> <node1> <node2>
            if (cmds.size() < 6) { cerr << "Error: 'add inductor' needs value, name, node1, node2." << endl; return true;}
            Inductor newInd;
            try { newInd.inductance = stod(cmds[2]); } catch (const std::exception& e) { cerr << "Error: Invalid inductance value for " << cmds[3] << endl; return true; }
            newInd.name = cmds[3];
            newInd.node1 = circuit.findNode(cmds[4]);
            newInd.node2 = circuit.findNode(cmds[5]);
            if (!newInd.node1 || !newInd.node2) { cerr << "Error: Node(s) not found for inductor " << newInd.name << endl; return true; }
            circuit.inductors.push_back(newInd);
        } else if (cmds[1] == "diode") { // add diode <name> <node1> <node2> [Is=<val>] [Vt=<val>] [n=<val>]
            if (cmds.size() < 5) { cerr << "Error: 'add diode' needs name, node1, node2." << endl; return true;}
            Diode newDiode;
            newDiode.name = cmds[2];
            newDiode.node1 = circuit.findNode(cmds[3]);
            newDiode.node2 = circuit.findNode(cmds[4]);
            if (!newDiode.node1 || !newDiode.node2) { cerr << "Error: Node(s) not found for diode " << newDiode.name << endl; return true; }
            // Optional parameters for diode
            for (size_t i = 5; i < cmds.size(); ++i) {
                string param = cmds[i];
                size_t eqPos = param.find('=');
                if (eqPos != string::npos) {
                    string key = param.substr(0, eqPos);
                    string valStr = param.substr(eqPos + 1);
                    try {
                        double val = stod(valStr);
                        if (key == "Is") newDiode.Is = val;
                        else if (key == "Vt") newDiode.Vt = val;
                        else if (key == "n") newDiode.n = val;
                    } catch (const std::exception& e) {
                        cerr << "Warning: Invalid value for diode parameter " << key << endl;
                    }
                }
            }
            circuit.diodes.push_back(newDiode);
        } else if (cmds[1] == "voltage" && cmds.size() > 2 && cmds[2] == "source") { // add voltage source <value> <node1_name> <node2_name> (VIN is main, this could be for others if generalized)
            if (cmds.size() < 6) { cerr << "Error: 'add voltage source' needs value, name (VIN implicit), node1, node2." << endl; return true;}
            // Assuming this command configures the main VIN source
            try { circuit.VIN.value = stod(cmds[3]); } catch (const std::exception& e) { cerr << "Error: Invalid voltage for VIN." << endl; return true; }
            circuit.VIN.node1 = circuit.findNode(cmds[4]); // Positive terminal
            circuit.VIN.node2 = circuit.findNode(cmds[5]); // Negative terminal
            if (!circuit.VIN.node1 || !circuit.VIN.node2) { cerr << "Error: Node(s) not found for VIN." << endl; return true;}
        } else if (cmds[1] == "current" && cmds.size() > 2 && cmds[2] == "source") { // add current source <value> <name> <node_from> <node_to>
            if (cmds.size() < 7) { cerr << "Error: 'add current source' needs value, name, node_from, node_to." << endl; return true;}
            CurrentSource newCS;
            try { newCS.value = stod(cmds[3]); } catch (const std::exception& e) { cerr << "Error: Invalid current value for " << cmds[4] << endl; return true; }
            newCS.name = cmds[4];
            newCS.node2 = circuit.findNode(cmds[5]); // Current flows FROM node2
            newCS.node1 = circuit.findNode(cmds[6]); // Current flows TO node1
            if (!newCS.node1 || !newCS.node2) { cerr << "Error: Node(s) not found for current source " << newCS.name << endl; return true; }
            circuit.currentSources.push_back(newCS);
        }
        else if (cmds[1] == "ground") { // add ground <node_name>
            if (cmds.size() < 3) { cerr << "Error: 'add ground' needs a node name." << endl; return true;}
            circuit.groundNodeNames.push_back(cmds[2]);
            Node* gndNode = circuit.findNode(cmds[2]);
            if (gndNode) {
                gndNode->setGround(true);
            } else {
                cerr << "Error: Node " << cmds[2] << " not found for ground connection." << endl;
            }
        } else {
            cerr << "Warning: Unknown 'add' command type: " << cmds[1] << endl;
        }
    } else if (cmds[0] == "read") { // read node voltage <name> | read current <comp_name> | read voltage <comp_name>
        readCommands.push_back(cmds);
    } else {
        cerr << "Warning: Unknown command: " << cmds[0] << endl;
    }
    return true; // Continue processing commands
}

// Main Function
int main() {
    Circuit circuit;
    string line;
    vector<vector<string>> readCommands; // Commands to execute after solving

    cout << fixed << setprecision(3); // Set precision for output

    // --- Input Phase ---
    // Reads circuit description from standard input
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
    handleErrors(circuit); // Call error handling function

    // --- DC Analysis (Simplified MNA in main) ---
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
        if (circuit.VIN.node1 && circuit.VIN.node2) {
            Node *vinN1 = circuit.VIN.node1; // Positive terminal
            Node *vinN2 = circuit.VIN.node2; // Negative terminal

            if (vinN1->isGround && !vinN2->isGround) { // Positive terminal grounded, negative is unknown
                int mx2 = nodeNumToMatrixIndex[vinN2->num];
                fill(A[mx2].begin(), A[mx2].end(), 0.0); // Clear row
                A[mx2][mx2] = 1.0;
                b[mx2] = -circuit.VIN.value; // V_n2 - V_n1(0) = -VIN.value  => V_n2 = -VIN.value
            } else if (!vinN1->isGround && vinN2->isGround) { // Negative terminal grounded, positive is unknown
                int mx1 = nodeNumToMatrixIndex[vinN1->num];
                fill(A[mx1].begin(), A[mx1].end(), 0.0); // Clear row
                A[mx1][mx1] = 1.0;
                b[mx1] = circuit.VIN.value; // V_n1 - V_n2(0) = VIN.value => V_n1 = VIN.value
            } else if (!vinN1->isGround && !vinN2->isGround) {
                cerr << "Warning: Floating voltage source VIN detected. Simplified MNA may not be accurate." << endl;
            }
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

        // Calculate current through VIN
        // I_VIN = Sum of currents leaving VIN.node1 through other components
        //     OR Sum of currents entering VIN.node2 through other components
        // This is complex if VIN is floating. If VIN.node2 is ground, I_VIN is sum of currents leaving VIN.node1.
        double i_vin_calc = 0.0;
        if (circuit.VIN.node1 && circuit.VIN.node2) {
            Node* n_plus_vin = circuit.VIN.node1; // Positive terminal of VIN
            // Sum currents leaving n_plus_vin through all components EXCEPT VIN itself
            for (auto& res : circuit.resistors) {
                if (res.node1 == n_plus_vin && res.node2 != circuit.VIN.node2) {
                    i_vin_calc += res.getCurrent();
                } else if (res.node2 == n_plus_vin && res.node1 != circuit.VIN.node2) {
                    i_vin_calc -= res.getCurrent(); // Current entering n_plus_vin
                }
            }
            for (auto& cs : circuit.currentSources) { // Current sources connected to n_plus_vin
                if (cs.node1 == n_plus_vin) i_vin_calc -= cs.getCurrent(); // CS injecting into n_plus_vin
                if (cs.node2 == n_plus_vin) i_vin_calc += cs.getCurrent(); // CS drawing from n_plus_vin
            }
            // Capacitors are open in DC.
            // Inductors are shorts in DC. If an inductor is connected to n_plus_vin, its current would contribute.
            // This simplified I_VIN calculation needs refinement for a general circuit.
            // The original code's I_VIN was also an approximation.
            // A full MNA would solve for source currents.
        }
        circuit.VIN.setCurrent(i_vin_calc); // Note: This calculation of i_vin is simplified.
        // A full MNA would solve for source currents.
    } else if (circuit.nodes.empty() && !circuit.groundNodeNames.empty()) {
        // Only ground nodes, all voltages are 0.
    } else if (circuit.nodes.empty()){
        // No nodes defined.
    } else {
        // All nodes are ground nodes.
        cout << "All nodes are ground. All voltages are 0." << endl;
    }


    // --- Output Phase ---
    for (auto &cmd_parts : readCommands) {
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
                cout << "VIN current = " << circuit.VIN.getCurrent() << " A" << endl;
            } else if (Resistor* r = circuit.findResistor(compName)) {
                cout << r->name << " current = " << fabs(r->getCurrent()) << " A" << endl;
            } else if (Capacitor* c = circuit.findCapacitor(compName)) {
                cout << c->name << " current (DC) = " << fabs(c->getCurrent()) << " A" << endl; // Should be 0 for DC
            } else if (Inductor* i = circuit.findInductor(compName)) {
                // Current through inductor in DC depends on the circuit.
                // The MNA here doesn't solve for it. We need to calculate it.
                // For now, if it's part of a simple loop, it might be calculable.
                // This is a placeholder.
                cout << i->name << " current (DC) = " << fabs(i->getCurrent()) << " A (Note: DC current in inductor requires specific calculation post-MNA or full MNA)" << endl;
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
                cout << "VIN voltage = " << circuit.VIN.getVoltage() << " V" << endl;
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

// Gaussian Elimination Implementation
vector<double> gaussianElimination(vector<vector<double>> A, vector<double> b_in) {
    vector<double> b = b_in; // Make a mutable copy
    int n = A.size();
    if (n == 0) return {};
    if (A[0].size() != (size_t)n || b.size() != (size_t)n) {
        cerr << "Error: Matrix dimensions mismatch in Gaussian elimination." << endl;
        return {}; // Invalid input
    }

    for (int i = 0; i < n; ++i) {
        // Find pivot row
        int maxR = i;
        for (int k = i + 1; k < n; ++k) {
            if (fabs(A[k][i]) > fabs(A[maxR][i])) {
                maxR = k;
            }
        }
        swap(A[i], A[maxR]);
        swap(b[i], b[maxR]);

        // Check for singular matrix (or near singular)
        if (fabs(A[i][i]) < 1e-9) { // Using a small epsilon
            cerr << "Warning: Matrix is singular or nearly singular during Gaussian elimination (pivot is near zero at row " << i << ")." << endl;
            // It might still produce a result, or one could return {} here.
            // For a circuit simulator, this often means an ill-defined circuit (e.g., floating parts, redundant sources).
            return {}; // Return empty indicating failure
        }

        // Eliminate forward
        for (int k = i + 1; k < n; ++k) {
            double factor = A[k][i] / A[i][i];
            for (int j = i; j < n; ++j) {
                A[k][j] -= factor * A[i][j];
            }
            b[k] -= factor * b[i];
        }
    }

    // Back substitution
    vector<double> x(n);
    for (int i = n - 1; i >= 0; --i) {
        if (fabs(A[i][i]) < 1e-9) { // Check again before division
            cerr << "Warning: Matrix is singular or nearly singular during back substitution (A[i][i] is near zero at row " << i << ")." << endl;
            return {}; // Return empty indicating failure
        }
        x[i] = b[i];
        for (int j = i + 1; j < n; ++j) {
            x[i] -= A[i][j] * x[j];
        }
        x[i] /= A[i][i];
    }
    return x;
}


// Stubs for other analysis functions and utilities

void transientAnalysis(Circuit& circuit, double tstep, double tstop) {
    cout << "// transientAnalysis function called (Not Implemented)" << endl;
    // This would involve:
    // 1. Discretizing d/dt for capacitors (I = C dV/dt) and inductors (V = L dI/dt).
    //    Common methods: Forward Euler, Backward Euler, Trapezoidal.
    //    Backward Euler is often preferred for stability.
    //    Example for Capacitor (Backward Euler): I_c(t) = C * (V_c(t) - V_c(t-dt)) / dt
    //    This means the capacitor behaves like a resistor R_eq = dt/C in series with a voltage source V_eq = V_c(t-dt).
    // 2. Setting up and solving the MNA equations at each time step from t=0 to t=tstop.
    // 3. Updating component states (capacitor voltages, inductor currents).
    // 4. Handling non-linear components like diodes, often requiring Newton-Raphson iteration at each time step.
}

void dcAnalysis(Circuit& circuit) {
    cout << "// dcAnalysis function called (Not Implemented - main has a simplified version)" << endl;
    // A more comprehensive DC analysis would:
    // 1. Handle non-linear components (like diodes) using iterative methods (e.g., Newton-Raphson).
    //    - Linearize the non-linear component around an operating point.
    //    - Solve the linear system.
    //    - Update the operating point.
    //    - Repeat until convergence.
    // 2. Robustly handle all ideal component types (floating voltage sources, inductors as shorts).
    // 3. Potentially perform DC sweeps (varying a source and calculating responses).
}

void handleErrors(const Circuit& circuit) {
    cout << "// handleErrors function called" << endl;
    if (circuit.groundNodeNames.empty() && !circuit.nodes.empty()) {
        cerr << "Error: No ground node detected in the circuit. Analysis may be unstable or incorrect." << endl;
    }
    for (const auto& res : circuit.resistors) {
        if (res.resistance <= 0) {
            cerr << "Error: Non-positive resistance value for resistor " << res.name << " (" << res.resistance << " Ohms)." << endl;
        }
        if (!res.node1 || !res.node2) {
            cerr << "Error: Resistor " << res.name << " has one or more null nodes." << endl;
        }
    }
    for (const auto& cap : circuit.capacitors) {
        if (cap.capacitance <= 0) {
            cerr << "Error: Non-positive capacitance value for capacitor " << cap.name << " (" << cap.capacitance << " F)." << endl;
        }
        if (!cap.node1 || !cap.node2) {
            cerr << "Error: Capacitor " << cap.name << " has one or more null nodes." << endl;
        }
    }
    for (const auto& ind : circuit.inductors) {
        if (ind.inductance <= 0) {
            cerr << "Error: Non-positive inductance value for inductor " << ind.name << " (" << ind.inductance << " H)." << endl;
        }
        if (!ind.node1 || !ind.node2) {
            cerr << "Error: Inductor " << ind.name << " has one or more null nodes." << endl;
        }
    }
    if (circuit.VIN.node1 && circuit.VIN.node2 && circuit.VIN.node1 == circuit.VIN.node2) {
        cerr << "Error: VIN source " << circuit.VIN.name << " has both terminals connected to the same node." << endl;
    }
    for(const auto& cs : circuit.currentSources) {
        if (cs.node1 && cs.node2 && cs.node1 == cs.node2) {
            cerr << "Error: Current source " << cs.name << " has both terminals connected to the same node." << endl;
        }
    }
    // Check for floating nodes (nodes not connected to anything or only to other floating nodes) - more complex check.
}

Circuit readCircuitFromFile(const string& filename) {
    cout << "// readCircuitFromFile function called with filename: " << filename << " (Not Implemented)" << endl;
    Circuit circuit;
    // Implementation would involve:
    // 1. Opening and reading the file line by line.
    // 2. Parsing commands similar to how cin is parsed in main.
    // 3. Populating the circuit object.
    // ifstream inputFile(filename);
    // if (!inputFile.is_open()) {
    //    cerr << "Error: Could not open circuit file " << filename << endl;
    //    return circuit; // Return empty circuit
    // }
    // string line;
    // while (getline(inputFile, line)) { ... parse line ... }
    // inputFile.close();
    return circuit;
}

void saveResultsToFile(const Circuit& circuit, const string& filename) {
    cout << "// saveResultsToFile function called with filename: " << filename << " (Not Implemented)" << endl;
    // Implementation would involve:
    // 1. Opening the file for writing.
    // 2. Iterating through solved node voltages and component currents/voltages.
    // 3. Writing them to the file in a structured format.
    // ofstream outputFile(filename);
    // if (!outputFile.is_open()) {
    //    cerr << "Error: Could not open file " << filename << " for saving results." << endl;
    //    return;
    // }
    // ... write data ...
    // outputFile.close();
}