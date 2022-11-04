#include "NeuralNetwork/Layer.h"

#include <iostream>
#include <map>

Layer::Layer(int size, NN::ActivationType type) {
    this->m_size = size;

    this->m_activateType = type;
    this->m_activate = NN::Activation::ActivateMap[type];
    this->m_derive = NN::Activation::DeriveMap[type];

    for (auto i = 0; i < size; i++) {
        m_neurons.push_back(std::make_shared<Neuron>(0.00));
    }
}

Layer::~Layer() {

    m_neurons.clear();
}

void Layer::setValAt(int index, double val) {
#if 0
    // derive value not use now
    this->m_neurons[index]->setVal(val, this->m_activate, this->m_derive);
#else
    this->m_neurons[index]->setVal(val, this->m_activate, nullptr);
#endif
}

double Layer::getValAt(int index) { return this->m_neurons[index]->getVal(); }

void Layer::setActivateType(NN::ActivationType type) {
    this->m_activateType = type;
    this->m_activate = NN::Activation::ActivateMap[type];
    this->m_derive = NN::Activation::DeriveMap[type];
}

std::shared_ptr<Matrix> Layer::valMatrix() {
    return buildMatrix([this]([[maybe_unused]] int row, int col) {
        return this->m_neurons.at(col)->getVal();
    });
}
std::shared_ptr<Matrix> Layer::activatedValMatrix() {
    return buildMatrix([this]([[maybe_unused]] int row, int col) {
        return this->m_neurons.at(col)->getActivatedVal();
    });
}
std::shared_ptr<Matrix> Layer::derivedValMatrix() {
    return buildMatrix([this]([[maybe_unused]] int row, int col) {
        return this->m_neurons.at(col)->getDerivedVal();
    });
}

std::shared_ptr<std::vector<double>> Layer::activatedValVector() {
    return buildVector([this](int index) {
        return this->m_neurons.at(index)->getActivatedVal();
    });
}

std::shared_ptr<Matrix> Layer::buildMatrix(const std::function<double(int row, int col)> &valueAt) {

    const int row = 1;
    const int col = this->m_neurons.size();

    std::shared_ptr<Matrix> m = std::make_shared<Matrix>(row, col, false);

    for (int r = 0; r < row; r++) {
        for (int c = 0; c < col; c++) {
            m->setValue(r, c, valueAt(r, c));
        }
    }

    return m;
}

std::shared_ptr<std::vector<double>> Layer::buildVector(const std::function<double(int index)> &valueAt) {

    std::shared_ptr<std::vector<double>> vec = std::make_shared<std::vector<double>>();
    vec->reserve(this->m_size);

    for (size_t i = 0; i < this->m_neurons.size(); i++) {
        vec->push_back(valueAt(i));
    }

    return vec;
}
