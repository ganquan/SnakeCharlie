#include "NeuralNetwork/NeuralNetwork.h"

#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

#include "NeuralNetwork/Neuron.h"
#include "NeuralNetwork/Utils.h"

using json = nlohmann::json;

NeuralNetwork::NeuralNetwork(const std::vector<int> &topology) {
    this->m_topology = topology;
    this->m_topologySize = topology.size();
    this->m_description = json{};

    initLayers();
    initWeightMatrices();
}

NeuralNetwork::NeuralNetwork(const std::string &filename) {

    std::ifstream i(filename);
    json nnJson;
    i >> nnJson;

    // topology
    this->m_topology = nnJson["topology"].get<std::vector<int>>();
    this->m_topologySize = this->m_topology.size();

    initLayers();
    initWeightMatrices(false);

    // description
    this->m_description = nnJson["description"];

    // weights
    std::vector<std::vector<std::vector<double>>> temp = nnJson["weights"];

    for (size_t i = 0; i < this->m_weightMatrices.size(); i++) {
        for (int r = 0; r < this->m_weightMatrices.at(i)->getRowNum(); r++) {
            for (int c = 0; c < this->m_weightMatrices.at(i)->getColNum(); c++) {
                this->m_weightMatrices.at(i)->setValue(r, c, temp.at(i).at(r).at(c));
            }
        }
    }

    i.close();
}

NeuralNetwork::~NeuralNetwork() {
    m_layers.clear();
    m_weightMatrices.clear();
}

void NeuralNetwork::initLayers() {

    m_layers.clear();
    for (int i = 0; i < m_topologySize; i++) {
        this->m_layers.push_back(std::make_shared<Layer>(m_topology[i]));
    }
}
void NeuralNetwork::initWeightMatrices(bool initWithRandom) {
    m_weightMatrices.clear();
    for (int i = 0; i < m_topologySize - 1; i++) {
        this->m_weightMatrices.push_back(std::make_shared<Matrix>(m_topology[i], m_topology[i + 1], initWithRandom));
    }
}

void NeuralNetwork::setWeightMatricesWithRandomValue() {
    for (size_t i = 0; i < this->m_weightMatrices.size(); i++) {
        this->m_weightMatrices.at(i)->fillWithRandom();
    }
}

void NeuralNetwork::feedForward() {

    for (int i = 0; i < (this->m_topologySize - 1); i++) {

        std::shared_ptr<Matrix> a = nullptr; // Matrix of neurons to the left
        std::shared_ptr<Matrix> b = nullptr; // Matrix of weights to the right of layer
        std::shared_ptr<Matrix> c = nullptr; // Matrix of neurons to the next layer

        if (i == 0) {
            a = this->valMatrixOfLayerAt(i);
        } else {
            a = this->activatedValMatrixOfLayerAt(i);
        }

        b = this->weightMatrixAt(i);
        c = std::make_shared<Matrix>(a->getRowNum(), b->getColNum(), false);

        NN::MatrixMath::multiply(a, b, c);

        for (int c_index = 0; c_index < c->getColNum(); c_index++) {

            this->setNeuronValue(i + 1, c_index, c->getValue(0, c_index) + this->m_bias);
        }
    }
}

void NeuralNetwork::setInput(const std::vector<double> &input) {
    this->m_input = input;

    for (size_t i = 0; i < input.size(); i++) {
        this->m_layers.at(0)->setValAt(i, input.at(i));
    }
}

void NeuralNetwork::loadNeuralNetwork(const std::string &filename) {
    std::ifstream i(filename);
    json nnJson;
    i >> nnJson;

    // topology
    this->m_topology = nnJson["topology"].get<std::vector<int>>();
    this->m_topologySize = this->m_topology.size();

    initLayers();
    initWeightMatrices(false);

    // description
    this->m_description = nnJson["description"];

    // weights
    std::vector<std::vector<std::vector<double>>> temp = nnJson["weights"];

    for (size_t i = 0; i < this->m_weightMatrices.size(); i++) {
        for (int r = 0; r < this->m_weightMatrices.at(i)->getRowNum(); r++) {
            for (int c = 0; c < this->m_weightMatrices.at(i)->getColNum(); c++) {
                this->m_weightMatrices.at(i)->setValue(r, c, temp.at(i).at(r).at(c));
            }
        }
    }

    i.close();
}

void NeuralNetwork::saveNeuralNetwork(const std::string &filename) {
    json nnJson = {};

    std::vector<std::vector<std::vector<double>>> weightSet;

    for (size_t i = 0; i < this->m_weightMatrices.size(); i++) {
        std::vector<std::vector<double>> &matrixPointer = this->m_weightMatrices.at(i)->getValues();
        weightSet.push_back(matrixPointer);
    }

    nnJson["description"] = this->m_description;
    nnJson["topology"] = this->m_topology;
    nnJson["weights"] = weightSet;

    std::ofstream o(filename);
    o << std::setw(4) << nnJson << std::endl;

    o.close();
}
