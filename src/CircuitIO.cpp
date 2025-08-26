#include "CircuitIO.h"
#include "Resistor.h"
#include "VoltageSource.h"
#include "Capacitor.h"
#include "Inductor.h"
#include "CurrentSource.h"
#include "Diode.h"
#include <sstream>
#include <map>
#include <cmath>

std::string CircuitIO::generateNetlist(const GraphicalCircuit& gCircuit) {
    std::stringstream netlist;
    std::map<std::pair<int, int>, int> nodeMap;
    int nodeCounter = 1;

    auto getNode = [&](double x, double y) {
        std::pair<int, int> pos = {static_cast<int>(round(x / 20)), static_cast<int>(round(y / 20))};
        if (nodeMap.find(pos) == nodeMap.end()) {
            nodeMap[pos] = nodeCounter++;
        }
        return nodeMap[pos];
    };

    for (const auto& wire : gCircuit.wires) {
        getNode(wire.first.first, wire.first.second);
        getNode(wire.second.first, wire.second.second);
    }

    for (const auto& comp : gCircuit.components) {
        // For simplicity, assuming components are 40x40 and centered
        double x = comp->x;
        double y = comp->y;
        int node1 = getNode(x - 20, y);
        int node2 = getNode(x + 20, y);

        netlist << comp->getName() << " " << node1 << " " << node2 << " " << comp->getValue() << std::endl;
    }

    return netlist.str();
}


Circuit CircuitIO::createCircuitFromNetlist(const std::string& netlist) {
    Circuit circuit;
    std::stringstream ss(netlist);
    std::string line;

    while (std::getline(ss, line)) {
        std::stringstream line_ss(line);
        std::string name;
        int node1_idx, node2_idx;
        double value;

        line_ss >> name >> node1_idx >> node2_idx >> value;

        Node* node1 = circuit.getNode(node1_idx);
        if (!node1) {
            node1 = new Node(node1_idx);
            circuit.addNode(node1);
        }

        Node* node2 = circuit.getNode(node2_idx);
        if (!node2) {
            node2 = new Node(node2_idx);
            circuit.addNode(node2);
        }
        
        if (name[0] == 'R') {
            circuit.addComponent(new Resistor(name, value, node1, node2));
        } else if (name[0] == 'V') {
            circuit.addComponent(new VoltageSource(name, value, node1, node2));
        } else if (name[0] == 'C') {
            circuit.addComponent(new Capacitor(name, value, node1, node2));
        } else if (name[0] == 'L') {
            circuit.addComponent(new Inductor(name, value, node1, node2));
        } else if (name[0] == 'I') {
            circuit.addComponent(new CurrentSource(name, value, node1, node2));
        } else if (name[0] == 'D') {
            circuit.addComponent(new Diode(name, node1, node2));
        }
    }

    return circuit;
}
