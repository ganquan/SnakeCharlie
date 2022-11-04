#pragma once

#include "SnakeDirection.h"
#include "SnakeModel.h"
#include "SnakeState.h"
#include <memory>
#include <vector>

class NeuralNetwork;
class PlayboardModel;

class SnakeBrain {

public:
    SnakeBrain();
    ~SnakeBrain();

    void setPlayboardModel(std::shared_ptr<PlayboardModel> &playboard);
    void buildVisionVector(std::vector<double> &visionVector, const BlockPosition &head, const BlockPosition &apple, const int row, const int col);

    SnakeDirection think(std::vector<double> &vision);

    std::shared_ptr<NeuralNetwork> getNeuralNetwork() { return m_nn; }

    void mutate(const std::vector<double> &mutateValueTable);
    static std::vector<std::vector<int>> visionChangeList;

private:
    inline double wallValue(const double wallDistance, const int totalBlockSize);
    inline double bodyValue(const double bodyDistance, const int totalBlockSize);
    inline double foodValue(const double foodDistance, const int totalBlockSize);

    SnakeDirection randomDirection();
    void initWeightMatrixLenList(std::vector<int> &list);
    void initLayerActivateType();

private:
    std::weak_ptr<PlayboardModel> m_playboard;

    // neural network
    std::shared_ptr<NeuralNetwork> m_nn;

    // GA mutate
    std::vector<int> m_weightMatrixLenList;
    int m_totalMutateLenght;
};
