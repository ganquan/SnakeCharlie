#pragma once

#include <functional>
#include <vector>

#include "Activate.h"
#include "Matrix.h"
#include "Neuron.h"
#include <memory>

class NeuronActivateType;

class Layer {
public:
    Layer(int size, NN::ActivationType type = NN::ActivationType::none);
    ~Layer();

    void setValAt(int index, double val);
    double getValAt(int index);

    std::shared_ptr<Matrix> valMatrix();
    std::shared_ptr<Matrix> activatedValMatrix();
    std::shared_ptr<Matrix> derivedValMatrix();

    std::shared_ptr<std::vector<double>> activatedValVector();
    void setActivateType(NN::ActivationType type);

private:
    std::shared_ptr<Matrix> buildMatrix(const std::function<double(int, int)> &valueAt);
    std::shared_ptr<std::vector<double>> buildVector(const std::function<double(int index)> &valueAt);

private:
    int m_size;
    std::vector<std::shared_ptr<Neuron>> m_neurons;

    ActivateFunction m_activate;
    ActivateFunction m_derive;
    NN::ActivationType m_activateType;
};
