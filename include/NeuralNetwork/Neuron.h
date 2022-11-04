#pragma once
#include "Activate.h"

class Neuron {
public:
    Neuron(double v) { this->val = v; };
    ~Neuron(){};

    void setVal(double v, const ActivateFunction &activate = nullptr, const ActivateFunction &derive = nullptr) {
        this->val = v;

        if (nullptr != activate) {
            this->activatedVal = activate(v);
        }

        if (nullptr != derive) {
            this->derivedVal = derive(v);
        }
    }

    double getVal() { return val; }
    double getActivatedVal() { return activatedVal; }
    double getDerivedVal() { return derivedVal; }

private:
    double val;
    double activatedVal;
    double derivedVal;
};
