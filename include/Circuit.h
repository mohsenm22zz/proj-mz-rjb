#pragma once

#include <vector>
#include <string>
#include <map>

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
    vector<Node> nodes;
    vector<Resistor> resistors;
    vector<Capacitor> capacitors;
    vector<Inductor> inductors;
    vector<Diode> diodes;
    vector<VoltageSource> voltageSources;
    vector<CurrentSource> currentSources;
    vector<string> groundNodeNames;

    double delta_t;
    double Tstep = 1e-4;
    double Tstop;
    double Tstart = 0;
    double maxstep;

    vector<vector<double>> MNA_A;
    vector<double> MNA_v;
    vector<double> MNA_x;

    Circuit();

    Node *findNode(const string &find_from_name);

    Node *findNodeByNum(int num_to_find);

    Resistor *findResistor(const string &find_from_name);

    Capacitor *findCapacitor(const string &find_from_name);

    Inductor *findInductor(const string &find_from_name);

    Diode *findDiode(const string &find_from_name);

    CurrentSource *findCurrentSource(const string &find_from_name);

    VoltageSource *findVoltageSource(const string &find_from_name);

    vector<vector<double>> G();

    vector<vector<double>> B();

    vector<vector<double>> C();

    vector<vector<double>> D();

    vector<double> J();

    vector<double> E();

    void set_MNA_A();

    void set_MNA_v();

    void set_MNA_x();

    void addNode(const string &name);

    void setDeltaT();

    bool isNodeNameGround(const string &node_name) const;

    int getNodeMatrixIndex(const Node *target_node_ptr) const;

    int countNonGroundNodes() const;

    void updateComponentStates();
};
