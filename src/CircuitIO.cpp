#include "CircuitIO.h"
#include "Node.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <cstdlib>

using namespace std;

double stonum(const string &s) {
    if (s.empty()) {
        throw invalid_argument("Empty value");
    }
    string nums = s;
    for (int i = 0; i < nums.size(); i++){
        if (nums[i]=='e'){
            string c_s = nums.substr(0,i);
            string power_s = nums.substr(i+1);
            try {
                double c = stod(c_s);
                double power = stod(power_s);
                return c* pow(10,power);
            }catch (const exception &e) {
                throw invalid_argument("Invalid number format in \"" + s + "\"");
            }
        }
    }
    char prefix = 0;
    if (!nums.empty() && isalpha(nums.back())) {
        prefix = tolower(nums.back());
        nums.pop_back();
        if (prefix == 'g' && !nums.empty() && tolower(nums.back()) == 'e') {
            if (nums.length() > 1 && tolower(nums[nums.length() - 2]) == 'm') {
                nums.resize(nums.length() - 2);
                prefix = 'M';
            }
        }
    }
    double val;
    try {
        val = stod(nums);
    } catch (const exception &e) {
        throw invalid_argument("Invalid number format in \"" + s + "\"");
    }
    switch (prefix) {
        case 'p':
            return val * 1e-12;
        case 'n':
            return val * 1e-9;
        case 'u':
            return val * 1e-6;
        case 'm':
            return val * 1e-3;
        case 'k':
            return val * 1e3;
        case 'M':
            return val * 1e6;
        default:
            return val;
    }
}

void clrscrr() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

bool command_handling(Circuit &circuit, const vector<string> &cmds, vector<vector<string>> &analysisCommands) {
    if (cmds.empty()) return true;

    string command = cmds[0];
    transform(command.begin(), command.end(), command.begin(), ::tolower);

    if (command == "exit" || command == ".run") {
        return false;
    }

    if (command == "add") {
        if (cmds.size() < 3) {
            cerr << "Error: Syntax error" << endl;
            return true;
        }
        string name = cmds[1];
        transform(name.begin(), name.end(), name.begin(), ::toupper);

        if (name == "GND") {
            if (cmds.size() != 3) {
                cerr << "Error: Syntax error" << endl;
                return true;
            }
            Node *gndNode = circuit.findOrCreateNode(cmds[2]);
            gndNode->setGround(true);
            if (find(circuit.groundNodeNames.begin(), circuit.groundNodeNames.end(), cmds[2]) ==
                circuit.groundNodeNames.end()) {
                circuit.groundNodeNames.push_back(cmds[2]);
            }
            return true;
        }

        if (cmds.size() != 5) {
            cerr << "Error: Syntax error" << endl;
            return true;
        }

        char compType = name[0];
        string compName = name;
        string node1_name = cmds[2];
        string node2_name = cmds[3];
        string value_str = cmds[4];

        switch (compType) {
            case 'R': {
                if (circuit.findResistor(compName)) {
                    cerr << "Error: Resistor " << compName << " already exists in the circuit" << endl;
                    return true;
                }
                Resistor r;
                try { r.resistance = stonum(value_str); } catch (const exception &e) {
                    cerr << "Error: Invalid resistance for " << compName << endl;
                    return true;
                }
                if (r.resistance <= 0) {
                    cerr << "Error: Resistance cannot be zero or negative" << endl;
                    return true;
                }
                r.name = compName;
                r.node1 = circuit.findOrCreateNode(node1_name);
                r.node2 = circuit.findOrCreateNode(node2_name);
                circuit.resistors.push_back(r);
                break;
            }
            case 'C': {
                if (circuit.findCapacitor(compName)) {
                    cerr << "Error: Capacitor " << compName << " already exists in the circuit" << endl;
                    return true;
                }
                Capacitor c;
                try { c.capacitance = stonum(value_str); } catch (const exception &e) {
                    cerr << "Error: Invalid capacitance for " << compName << endl;
                    return true;
                }
                if (c.capacitance <= 0) {
                    cerr << "Error: Capacitance cannot be zero or negative" << endl;
                    return true;
                }
                c.name = compName;
                c.node1 = circuit.findOrCreateNode(node1_name);
                c.node2 = circuit.findOrCreateNode(node2_name);
                circuit.capacitors.push_back(c);
                break;
            }
            case 'L': {
                if (circuit.findInductor(compName)) {
                    cerr << "Error: inductor " << compName << " already exists in the circuit" << endl;
                    return true;
                }
                Inductor l;
                try { l.inductance = stonum(value_str); } catch (const exception &e) {
                    cerr << "Error: Invalid inductance for " << compName << endl;
                    return true;
                }
                if (l.inductance <= 0) {
                    cerr << "Error: Inductance cannot be zero or negative" << endl;
                    return true;
                }
                l.name = compName;
                l.node1 = circuit.findOrCreateNode(node1_name);
                l.node2 = circuit.findOrCreateNode(node2_name);
                circuit.inductors.push_back(l);
                break;
            }
            case 'D': {
                if (circuit.findDiode(compName)) {
                    cerr << "Error: diode " << compName << " already exists in the circuit" << endl;
                    return true;
                }
                double forward_voltage = 0.0;
                try { forward_voltage = stonum(value_str); } catch (const exception &e) {
                    cerr << "Error: Invalid voltage for " << compName << endl;
                    return true;
                }
                if (forward_voltage < 0) {
                    cerr << "Error: Forward voltage cannot be negative" << endl;
                    return true;
                }

                Diode d(compName, circuit.findOrCreateNode(node1_name), circuit.findOrCreateNode(node2_name), NORMAL,
                        forward_voltage);
                circuit.diodes.push_back(d);
                break;
            }
            case 'V': {
                if (circuit.findVoltageSource(compName)) {
                    cerr << "Error: Voltage source " << compName << " already exists in the circuit" << endl;
                    return true;
                }
                VoltageSource vs;
                try { vs.value = stonum(value_str); } catch (const exception &e) {
                    cerr << "Error: Invalid voltage value for " << compName << endl;
                    return true;
                }
                vs.name = compName;
                vs.node1 = circuit.findOrCreateNode(node1_name);
                vs.node2 = circuit.findOrCreateNode(node2_name);
                circuit.voltageSources.push_back(vs);
                break;
            }
            case 'I': {
                if (circuit.findCurrentSource(compName)) {
                    cerr << "Error: Current source " << compName << " already exists in the circuit" << endl;
                    return true;
                }
                CurrentSource cs;
                try { cs.value = stonum(value_str); } catch (const exception &e) {
                    cerr << "Error: Invalid current value for " << compName << endl;
                    return true;
                }
                cs.name = compName;
                cs.node1 = circuit.findOrCreateNode(node1_name);
                cs.node2 = circuit.findOrCreateNode(node2_name);
                circuit.currentSources.push_back(cs);
                break;
            }
            default:
                cerr << "Error: Unknown component type for 'add': " << name << endl;
                break;
        }
    } else if (command == "delete") {
        if (cmds.size() != 2) {
            cerr << "Error: Syntax error" << endl;
            return true;
        }
        string compName = cmds[1];
        transform(compName.begin(), compName.end(), compName.begin(), ::toupper);
        char compTypePrefix = compName[0];
        bool deleted = false;

        switch (compTypePrefix) {
            case 'R':
                deleted = circuit.deleteResistor(compName);
                if (!deleted) cerr << "Error: Cannot delete resistor; component not found" << endl;
                break;
            case 'C':
                deleted = circuit.deleteCapacitor(compName);
                if (!deleted) cerr << "Error: Cannot delete capacitor; component not found" << endl;
                break;
            case 'L':
                deleted = circuit.deleteInductor(compName);
                if (!deleted) cerr << "Error: Cannot delete inductor; component not found" << endl;
                break;
            case 'D':
                deleted = circuit.deleteDiode(compName);
                if (!deleted) cerr << "Error: Cannot delete diode; component not found" << endl;
                break;
            case 'V':
                deleted = circuit.deleteVoltageSource(compName);
                if (!deleted) cerr << "Error: Cannot delete voltage source; component not found" << endl;
                break;
            case 'I':
                deleted = circuit.deleteCurrentSource(compName);
                if (!deleted) cerr << "Error: Cannot delete current source; component not found" << endl;
                break;
            default:
                cerr << "Error: Unknown component prefix for deletion: " << compTypePrefix << endl;
                return true;
        }
    } else if (command == ".nodes") {
        cout << "Available nodes:" << endl;
        for (const auto *node: circuit.nodes) {
            cout << node->name << " ";
        }
        cout << endl;
    } else if (command == "list") {
        if (cmds.size() < 2) {
            cout << "Components in circuit:" << endl;
            for (const auto &comp: circuit.resistors) { cout << "Resistor: " << comp.name << endl; }
            for (const auto &comp: circuit.capacitors) { cout << "Capacitor: " << comp.name << endl; }
            for (const auto &comp: circuit.inductors) { cout << "Inductor: " << comp.name << endl; }
            for (const auto &comp: circuit.diodes) { cout << "Diode: " << comp.name << endl; }
            for (const auto &comp: circuit.voltageSources) { cout << "VoltageSource: " << comp.name << endl; }
            for (const auto &comp: circuit.currentSources) { cout << "CurrentSource: " << comp.name << endl; }
        } else if (cmds[1] == "Resistor") {
            for (const auto &comp: circuit.resistors) { cout << "Resistor: " << comp.name << endl; }
        } else if (cmds[1] == "Capacitor") {
            for (const auto &comp: circuit.capacitors) { cout << "Capacitor: " << comp.name << endl; }
        } else if (cmds[1] == "Inductor") {
            for (const auto &comp: circuit.inductors) { cout << "Inductor: " << comp.name << endl; }
        } else if (cmds[1] == "Diode") {
            for (const auto &comp: circuit.diodes) { cout << "Diode: " << comp.name << endl; }
        } else if (cmds[1] == "VoltageSources") {
            for (const auto &comp: circuit.voltageSources) { cout << "VoltageSource: " << comp.name << endl; }
        } else if (cmds[1] == "CurrentSources") {
            for (const auto &comp: circuit.currentSources) { cout << "CurrentSource: " << comp.name << endl; }
        } else {
            cerr << "Error: Syntax error" << endl;
        }
    } else if (command == "rename") {
        if (cmds.size() != 3) {
            cerr << "Error: Syntax error. Usage: rename <old_node_name> <new_node_name>" << endl;
            return true;
        }
        string oldName = cmds[1];
        string newName = cmds[2];
        bool oldNameNotFound = false;
        bool newNameExists = false;
        bool isGround = false;
        bool success = circuit.renameNode(oldName, newName, oldNameNotFound, newNameExists, isGround);
        if (success) {
            cout << "Node '" << oldName << "' was successfully renamed to '" << newName << "'." << endl;
        } else {
            if (oldNameNotFound) {
                cerr << "Error: Node '" << oldName << "' does not exist." << endl;
            } else if (newNameExists) {
                cerr << "Error: A node with the name '" << newName << "' already exists." << endl;
            } else if (isGround) {
                cerr << "Error: The ground node cannot be renamed." << endl;
            }
        }
    } else if (command == ".dc" || command == ".tran") {
        analysisCommands.push_back(cmds);
    } else {
        cerr << "Error: Unknown command: " << cmds[0] << endl;
    }
    return true;
}

bool handleErrors(const Circuit &circuit) {
    if (circuit.groundNodeNames.empty() && !circuit.nodes.empty()) {
        cerr << "Error: Circuit not valid." << endl;
        return false;
    }
    vector<Node *> connected;
    for (Resistor comp: circuit.resistors) {
        bool added1 = false, added2 = false;
        for (Node *n: connected) {
            if (comp.node1 == n) {
                added1 = true;
            }
        }
        for (Node *n: connected) {
            if (comp.node2 == n) {
                added2 = true;
            }
        }
        if (!added1)
            connected.push_back(comp.node1);
        if (!added2)
            connected.push_back(comp.node2);
    }
    for (Capacitor comp: circuit.capacitors) {
        bool added1 = false, added2 = false;
        for (Node *n: connected) {
            if (comp.node1 == n) {
                added1 = true;
            }
        }
        for (Node *n: connected) {
            if (comp.node2 == n) {
                added2 = true;
            }
        }
        if (!added1)
            connected.push_back(comp.node1);
        if (!added2)
            connected.push_back(comp.node2);
    }
    for (Inductor comp: circuit.inductors) {
        bool added1 = false, added2 = false;
        for (Node *n: connected) {
            if (comp.node1 == n) {
                added1 = true;
            }
        }
        for (Node *n: connected) {
            if (comp.node2 == n) {
                added2 = true;
            }
        }
        if (!added1)
            connected.push_back(comp.node1);
        if (!added2)
            connected.push_back(comp.node2);
    }
    for (Diode comp: circuit.diodes) {
        bool added1 = false, added2 = false;
        for (Node *n: connected) {
            if (comp.node1 == n) {
                added1 = true;
            }
        }
        for (Node *n: connected) {
            if (comp.node2 == n) {
                added2 = true;
            }
        }
        if (!added1)
            connected.push_back(comp.node1);
        if (!added2)
            connected.push_back(comp.node2);
    }
    for (VoltageSource comp: circuit.voltageSources) {
        bool added1 = false, added2 = false;
        for (Node *n: connected) {
            if (comp.node1 == n) {
                added1 = true;
            }
        }
        for (Node *n: connected) {
            if (comp.node2 == n) {
                added2 = true;
            }
        }
        if (!added1)
            connected.push_back(comp.node1);
        if (!added2)
            connected.push_back(comp.node2);
    }
    for (CurrentSource comp: circuit.currentSources) {
        bool added1 = false, added2 = false;
        for (Node *n: connected) {
            if (comp.node1 == n) {
                added1 = true;
            }
        }
        for (Node *n: connected) {
            if (comp.node2 == n) {
                added2 = true;
            }
        }
        if (!added1)
            connected.push_back(comp.node1);
        if (!added2)
            connected.push_back(comp.node2);
    }
    if (connected.size()<circuit.nodes.size()+circuit.countNonGroundNodes()){
        cerr << "Error: Circuit not valid." << endl;
        return false;
    }
    return true;
}

Circuit readCircuitFromFile(const string &filename) {
    Circuit circuit;
    ifstream inputFile(filename);
    if (!inputFile.is_open()) {
        cerr << "Error: Could not open circuit file " << filename << endl;
        return circuit;
    }
    string line;
    vector<vector<string>> analysisCmds;
    while (getline(inputFile, line)) {
        istringstream iss(line);
        vector<string> cmds;
        string cmd;
        while (iss >> cmd) {
            cmds.push_back(cmd);
        }
        if (!cmds.empty()) {
            command_handling(circuit, cmds, analysisCmds);
        }
    }
    inputFile.close();
    handleErrors(circuit);
    return circuit;
}

void saveResultsToFile(const Circuit &circuit, const string &filename) {
    cout << "// saveResultsToFile function called with filename: " << filename << endl;
}