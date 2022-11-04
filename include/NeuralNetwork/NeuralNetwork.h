#pragma once

#include <string>
#include <vector>

#include "Activate.h"
#include "Layer.h"
#include "Matrix.h"
#include <memory>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

class NeuralNetwork {
public:
    NeuralNetwork(const std::vector<int> &topology);
    NeuralNetwork(const std::string &filename);
    ~NeuralNetwork();

    void setInput(const std::vector<double> &input);
    void feedForward();

public:
    void setWeightMatricesWithRandomValue();
    void setNeuronValue(int indexLayer, int indexNeuron, double val) { this->m_layers.at(indexLayer)->setValAt(indexNeuron, val); }
    void setLayerActivateType(int indexLayer, const NN::ActivationType &type) { this->m_layers.at(indexLayer)->setActivateType(type); }

    const std::vector<int> &getTopology() { return m_topology; }

    void setDescription(const json &description) { this->m_description = description; }
    const json &getDescription() { return this->m_description; }

public:
    std::shared_ptr<std::vector<double>> activatedValVectorOfLayerAt(int index) { return this->m_layers.at(index)->activatedValVector(); }

    std::shared_ptr<Matrix> valMatrixOfLayerAt(int index) { return this->m_layers.at(index)->valMatrix(); }
    std::shared_ptr<Matrix> activatedValMatrixOfLayerAt(int index) { return this->m_layers.at(index)->activatedValMatrix(); }
    std::shared_ptr<Matrix> derivedValMatrixOfLayerAt(int index) { return this->m_layers.at(index)->derivedValMatrix(); }

    std::shared_ptr<Matrix> weightMatrixAt(int index) { return this->m_weightMatrices.at(index); };

public:
    void loadNeuralNetwork(const std::string &filename);
    void saveNeuralNetwork(const std::string &filename);

private:
    void initLayers();
    void initWeightMatrices(bool initWithRandom = false);

private:
    std::vector<double> m_input;
    double m_bias = 1.0;

    json m_description;

    int m_topologySize;
    std::vector<int> m_topology;
    std::vector<std::shared_ptr<Layer>> m_layers;
    std::vector<std::shared_ptr<Matrix>> m_weightMatrices;
};
