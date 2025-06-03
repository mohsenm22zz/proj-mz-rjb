#pragma once

#include <vector>
#include <string>
#include <map>

#include "Node.h"
#include "Resistor.h"
#include "Capacitor.h"
#include "Inductor.h"
#include "Diode.h"
#include "VoltageSource.h"
#include "CurrentSource.h"

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

    vector<vector<double>> J();

    vector<vector<double>> E();

    vector<vector<double>> MNA_A;
    vector<double> MNA_v;
    vector<double> MNA_x;

    void set_MNA_A();

    void set_MNA_v();

    void set_MNA_x();

    void addNode(const string &name);
};