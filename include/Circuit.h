#pragma once

#include <vector>
#include <string>
#include <list>
#include <map>
#include <memory>

class Node;
class Component;

#include "Node.h"
#include "Resistor.h"
#include "Capacitor.h"
#include "Inductor.h"
#include "Diode.h"
#include "VoltageSource.h"
#include "CurrentSource.h"
#include "Component.h"

using namespace std;

class Circuit {
public:
    vector<Node*> nodes;
    vector<Resistor> resistors;
    vector<Capacitor> capacitors;
    vector<Inductor> inductors;
    vector<Diode> diodes;
    vector<VoltageSource> voltageSources;
    vector<CurrentSource> currentSources;
    vector<string> groundNodeNames;

    double delta_t;
    vector<vector<double>> MNA_A;
    vector<double> MNA_RHS;
    vector<double> MNA_solution;

    Circuit();
    ~Circuit();

    void addNode(const string& name);
    Node* findNode(const string& name);
    Node* findOrCreateNode(const string& name);
    Node* findNodeByNum(int num_to_find);

    Resistor* findResistor(const string& name);
    Capacitor* findCapacitor(const string& name);
    Inductor* findInductor(const string& name);
    Diode* findDiode(const string& name);
    CurrentSource* findCurrentSource(const string& name);
    VoltageSource* findVoltageSource(const string& name);

    bool deleteResistor(const string& name);
    bool deleteCapacitor(const string& name);
    bool deleteInductor(const string& name);
    bool deleteDiode(const string& name);
    bool deleteVoltageSource(const string& name);
    bool deleteCurrentSource(const string& name);

    vector<vector<double>> G();
    vector<vector<double>> B();
    vector<vector<double>> C();
    vector<vector<double>> D();
    vector<double> J();
    vector<double> E();

    void set_MNA_A();
    void set_MNA_RHS();
    void MNA_sol_size();

    void setDeltaT(double dt);
    void updateComponentStates();

    void clearComponentHistory();

    bool isNodeNameGround(const string& node_name) const;
    int getNodeMatrixIndex(const Node* target_node_ptr) const;
    int countNonGroundNodes() const;

    int countTotalExtraVariables();
    void assignDiodeBranchIndices();
};