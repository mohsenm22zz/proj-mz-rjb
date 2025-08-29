#ifndef ACVOLTAGESOURCE_H
#define ACVOLTAGESOURCE_H

#include "Component.h"
#include "Node.h"

class ACVoltageSource : public Component {
public:
    double magnitude = 0.0;
    double frequency = 0.0;
    double phase = 0.0;
    Node* node1 = nullptr;
    Node* node2 = nullptr;

    double getValue(double time) const;
};

#endif
