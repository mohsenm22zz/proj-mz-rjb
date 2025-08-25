#pragma once

#include <vector>
#include <string>
#include <list>
#include <map>
#include <memory>
#include <complex> // Include for complex number support

class Node;
class Component;

#include "Node.h"
#include "Resistor.h"
#include "Capacitor.h"
#include "Inductor.h"
#include "Diode.h"
#include "VoltageSource.h"
#include "CurrentSource.h"
#include "ACVoltageSource.h" // --- NEW ---
#include "Component.h"

using namespace std;

// --- NEW ---
// Enum to specify the type of analysis to perform
enum class AnalysisType {
    DC,
    TRANSIENT,
    AC_SWEEP
};


class Circuit {
public:
    vector<Node*> nodes;
    vector<Resistor> resistors;
    vector<Capacitor> capacitors;
    vector<Inductor> inductors;
    vector<Diode> diodes;
    vector<VoltageSource> voltageSources;
    vector<ACVoltageSource> acVoltageSources; // --- NEW ---
    vector<CurrentSource> currentSources;
    vector<string> groundNodeNames;

    double delta_t;

    // MNA matrices for DC/Transient (real numbers)
    vector<vector<double>> MNA_A;
    vector<double> MNA_RHS;

    // --- NEW ---
    // MNA matrices for AC analysis (complex numbers)
    vector<vector<complex<double>>> MNA_A_Complex;
    vector<complex<double>> MNA_RHS_Complex;


    Circuit();
    ~Circuit();

    void addNode(const string& name);
    Node* findNode(const string& name);
    Node* findOrCreateNode(const string& name);

    // ... findComponent functions ...
    Resistor* findResistor(const string& name);
    Capacitor* findCapacitor(const string& name);
    Inductor* findInductor(const string& name);
    Diode* findDiode(const string& name);
    CurrentSource* findCurrentSource(const string& name);
    VoltageSource* findVoltageSource(const string& name);
    ACVoltageSource* findACVoltageSource(const string& name); // --- NEW ---


    // ... deleteComponent functions ...
    bool deleteResistor(const string& name);
    // ... other delete functions ...

    // --- MODIFIED ---
    // The MNA setup functions now need to know the analysis type and frequency for AC
    void set_MNA_A(AnalysisType type, double frequency = 0);
    void set_MNA_RHS(AnalysisType type, double frequency = 0);

    // ... other existing functions ...
    void setDeltaT(double dt);
    void updateComponentStates();
    void clearComponentHistory();
    int getNodeMatrixIndex(const Node* target_node_ptr) const;
    int countNonGroundNodes() const;
    int countTotalExtraVariables();
    void assignDiodeBranchIndices();
};
