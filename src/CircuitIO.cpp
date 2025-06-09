#include "CircuitIO.h"
#include "Node.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <cstdlib>
using namespace std;

double stonum(const string& s) {
    if (s.empty()) {
        throw invalid_argument("Empty value string");
    }
    string nums = s;
    char prefix = 0;
    if (!nums.empty() && isalpha(nums.back())) {
        prefix = tolower(nums.back());
        nums.pop_back();
        if (prefix == 'g' && !nums.empty() && tolower(nums.back()) == 'e') {
            if (nums.length() > 1 && tolower(nums[nums.length()-2]) == 'm'){
                nums.resize(nums.length() - 2);
                prefix = 'M';
            }
        }
    }
    double val;
    try {
        val = stod(nums);
    } catch (const exception& e) {
        throw invalid_argument("Invalid number format in \"" + s + "\"");
    }
    switch (prefix) {
        case 'p': return val * 1e-12;
        case 'n': return val * 1e-9;
        case 'u': return val * 1e-6;
        case 'm': return val * 1e-3;
        case 'k': return val * 1e3;
        case 'M': return val * 1e6;
        default: return val;
    }
}

void clrscrr(){
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}
bool command_handling(Circuit& circuit, const vector<string>& cmds, vector<vector<string>>& analysisCommands) {
    if (cmds.empty()) return true;
    string command = cmds[0];
    transform(command.begin(), command.end(), command.begin(), ::tolower);
    if (command == "exit" or command == ".run") {
        return false;
    } else if (command == "add") {
        if (cmds.size() < 3) { cerr << "Error: 'add' command is incomplete. Expected: add <type> ..." << endl; return true;}
        string type1 = cmds[1];
        transform(type1.begin(), type1.end(), type1.begin(), ::tolower);
        if (type1 == "voltage" && cmds.size() > 2 && tolower(cmds[2][0]) == 's') { // "voltage source"
            if (cmds.size() != 7) { cerr << "Error: Incorrect arguments for 'add voltage source'. Expected: add voltage source <name> <node+> <node-> <value>" << endl; return true;}
            if (circuit.findVoltageSource(cmds[3])) { cerr << "Error: Voltage source '" << cmds[3] << "' already exists." << endl; return true; }
            VoltageSource newVS;
            try { newVS.value = stonum(cmds[6]); } catch (const exception& e) { cerr << "Error: Invalid voltage value for " << cmds[3] << ": " << e.what() << endl; return true; }
            newVS.name = cmds[3];
            newVS.node1 = circuit.findOrCreateNode(cmds[4]);
            newVS.node2 = circuit.findOrCreateNode(cmds[5]);
            circuit.voltageSources.push_back(newVS);
            return true;
        } else if (type1 == "current" && cmds.size() > 2 && tolower(cmds[2][0]) == 's') { // "current source"
            if (cmds.size() != 7) { cerr << "Error: Incorrect arguments for 'add current source'. Expected: add current source <name> <node_from> <node_to> <value>" << endl; return true;}
            if (circuit.findCurrentSource(cmds[3])) { cerr << "Error: Current source '" << cmds[3] << "' already exists." << endl; return true; }
            CurrentSource newCS;
            try { newCS.value = stonum(cmds[6]); } catch (const exception& e) { cerr << "Error: Invalid current value for " << cmds[3] << ": " << e.what() << endl; return true; }
            newCS.name = cmds[3];
            newCS.node2 = circuit.findOrCreateNode(cmds[4]);
            newCS.node1 = circuit.findOrCreateNode(cmds[5]);
            circuit.currentSources.push_back(newCS);
            return true;
        }
        if (type1 == "resistor") {
            if (cmds.size() != 6) { cerr << "Error: Incorrect arguments for 'add resistor'. Expected: add resistor <name> <node1> <node2> <value>" << endl; return true;}
            if (circuit.findResistor(cmds[2])) { cerr << "Error: Resistor '" << cmds[2] << "' already exists." << endl; return true; }
            Resistor r;
            try { r.resistance = stonum(cmds[5]); } catch (const exception& e) { cerr << "Error: Invalid resistance for " << cmds[2] << ": " << e.what() << endl; return true; }
            if (r.resistance <= 0) { cerr << "Error: Resistance for " << cmds[2] << " must be positive." << endl; return true; }
            r.name = cmds[2];
            r.node1 = circuit.findOrCreateNode(cmds[3]);
            r.node2 = circuit.findOrCreateNode(cmds[4]);
            circuit.resistors.push_back(r);
        } else if (type1 == "capacitor") {
            if (cmds.size() != 6) { cerr << "Error: Incorrect arguments for 'add capacitor'. Expected: add capacitor <name> <node1> <node2> <value>" << endl; return true;}
            if (circuit.findCapacitor(cmds[2])) { cerr << "Error: Capacitor '" << cmds[2] << "' already exists." << endl; return true; }
            Capacitor c;
            try { c.capacitance = stonum(cmds[5]); } catch (const exception& e) { cerr << "Error: Invalid capacitance for " << cmds[2] << ": " << e.what() << endl; return true; }
            if (c.capacitance <= 0) { cerr << "Error: Capacitance for " << cmds[2] << " must be positive." << endl; return true; }
            c.name = cmds[2];
            c.node1 = circuit.findOrCreateNode(cmds[3]);
            c.node2 = circuit.findOrCreateNode(cmds[4]);
            circuit.capacitors.push_back(c);
        } else if (type1 == "inductor") {
            if (cmds.size() != 6) { cerr << "Error: Incorrect arguments for 'add inductor'. Expected: add inductor <name> <node1> <node2> <value>" << endl; return true;}
            if (circuit.findInductor(cmds[2])) { cerr << "Error: Inductor '" << cmds[2] << "' already exists." << endl; return true; }
            Inductor l;
            try { l.inductance = stonum(cmds[5]); } catch (const exception& e) { cerr << "Error: Invalid inductance for " << cmds[2] << ": " << e.what() << endl; return true; }
            if (l.inductance <= 0) { cerr << "Error: Inductance for " << cmds[2] << " must be positive." << endl; return true; }
            l.name = cmds[2];
            l.node1 = circuit.findOrCreateNode(cmds[3]);
            l.node2 = circuit.findOrCreateNode(cmds[4]);
            circuit.inductors.push_back(l);
        } else if (type1 == "diode") {
            if (cmds.size() != 5) { cerr << "Error: Incorrect arguments for 'add diode'. Expected: add diode <name> <node1> <node2>" << endl; return true;}
            if (circuit.findDiode(cmds[2])) { cerr << "Error: Diode '" << cmds[2] << "' already exists." << endl; return true; }
            Diode d;
            d.name = cmds[2];
            d.node1 = circuit.findOrCreateNode(cmds[3]);
            d.node2 = circuit.findOrCreateNode(cmds[4]);
            circuit.diodes.push_back(d);
        } else if (type1 == "ground") {
            if (cmds.size() != 3) { cerr << "Error: 'add ground' needs a node name." << endl; return true;}
            Node* gndNode = circuit.findOrCreateNode(cmds[2]);
            gndNode->setGround(true);
            if (find(circuit.groundNodeNames.begin(), circuit.groundNodeNames.end(), cmds[2]) == circuit.groundNodeNames.end()) {
                circuit.groundNodeNames.push_back(cmds[2]);
            }
        }
        else {
            cerr << "Error: Unknown component type for 'add': " << cmds[1] << endl;
        }

    } else if (command == "delete") {
        if (cmds.size() != 2) { cerr << "Error: 'delete' requires exactly one argument: <ComponentName>" << endl; return true; }
        string compName = cmds[1];
        char compTypePrefix = toupper(compName[0]);
        bool deleted = false;
        switch (compTypePrefix) {
            case 'R': deleted = circuit.deleteResistor(compName); break;
            case 'C': deleted = circuit.deleteCapacitor(compName); break;
            case 'L': deleted = circuit.deleteInductor(compName); break;
            case 'D': deleted = circuit.deleteDiode(compName); break;
            case 'V': deleted = circuit.deleteVoltageSource(compName); break;
            case 'I': deleted = circuit.deleteCurrentSource(compName); break;
            default:
                cerr << "Error: Unknown component prefix for deletion: " << compTypePrefix << endl;
                return true;
        }
        if (deleted) {
            cout << "Component '" << compName << "' deleted successfully." << endl;
        } else {
            cerr << "Error: Cannot delete component '" << compName << "'; component not found." << endl;
        }
    } else if (command == ".dc" || command == ".tran") {
        analysisCommands.push_back(cmds);
    } else {
        cerr << "Error: Unknown command: " << cmds[0] << endl;
    }
    return true;
}
void handleErrors(const Circuit& circuit) {
    if (circuit.groundNodeNames.empty() && !circuit.nodes.empty()) {
        cerr << "Error: No ground node detected in the circuit. Analysis may be unstable or incorrect." << endl;
    }
    cout << "// Circuit integrity check complete." << endl;
}

Circuit readCircuitFromFile(const string& filename) {
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
        while(iss >> cmd){
            cmds.push_back(cmd);
        }
        if(!cmds.empty()){
            command_handling(circuit, cmds, analysisCmds);
        }
    }
    inputFile.close();
    handleErrors(circuit);
    return circuit;
}

void saveResultsToFile(const Circuit& circuit, const string& filename) {
    cout << "// saveResultsToFile function called with filename: " << filename << " (Not Implemented)" << endl;
}
