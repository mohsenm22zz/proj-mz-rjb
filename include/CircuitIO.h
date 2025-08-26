#ifndef CIRCUITIO_H
#define CIRCUITIO_H

#include "Circuit.h"
#include <string>
#include <vector>
#include <memory>

// A structure to hold the graphical information of the circuit
struct GraphicalCircuit {
    std::vector<std::shared_ptr<Component>> components;
    // Wires can be represented by a list of start and end points
    std::vector<std::pair<std::pair<double, double>, std::pair<double, double>>> wires;
};

class CircuitIO {
public:
    static std::string generateNetlist(const GraphicalCircuit& gCircuit);
    static Circuit createCircuitFromNetlist(const std::string& netlist);
};

#endif // CIRCUITIO_H
