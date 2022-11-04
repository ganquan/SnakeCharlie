#include "SnakeBrain.h"

#include "AppConfig.h"
#include "NeuralNetwork/Activate.h"
#include "NeuralNetwork/NeuralNetwork.h"
#include "Utility.h"
#include <filesystem>
#include <glog/logging.h>

#include "PlayboardModel.h"
#include <fmt/core.h>
#include <fmt/ranges.h>
#include <limits>

std::vector<std::vector<int>> SnakeBrain::visionChangeList{
    std::vector<int>{-1, 0},  /* 00:00 */
    std::vector<int>{-1, 1},  /* 01:30 */
    std::vector<int>{0, 1},   /* 03:00 */
    std::vector<int>{1, 1},   /* 04:30 */
    std::vector<int>{1, 0},   /* 06:00 */
    std::vector<int>{1, -1},  /* 07:30 */
    std::vector<int>{0, -1},  /* 09:00 */
    std::vector<int>{-1, -1}, /* 10:30 */
};

SnakeBrain::SnakeBrain() {

    std::string nnFileName = AppConfig::NeuralNetworkFilename();

    if (AppConfig::RunMode().isAIMode()) {

        if (std::filesystem::exists(nnFileName)) {
            LOG(INFO) << "SnakeBrain Load nn from file: " << nnFileName;
            m_nn = std::make_unique<NeuralNetwork>(nnFileName);
        } else {
            LOG(INFO) << "SnakeBrain Init nn from topology with random weights";
            m_nn = std::make_unique<NeuralNetwork>(AppConfig::TrainingTopology());
            m_nn->setWeightMatricesWithRandomValue();

            std::string nnWeightFilePath = std::string("../config/");
            std::string nnWeightFileName = std::string("network");
            std::string nnWeightFileNameSuffix = std::string("");
            std::string nnWeightFileExtension = std::string(".json");

            std::string nnFileName = AppConfig::NeuralNetworkFilename();

            int counter = 0;
            while (std::filesystem::exists(nnFileName)) {
                nnWeightFileNameSuffix = std::to_string(counter);
                nnFileName = nnWeightFilePath + nnWeightFileName + nnWeightFileNameSuffix + nnWeightFileExtension;
                counter++;
            }

            m_nn->saveNeuralNetwork(nnFileName);

            AppConfig::Get().setNeuralNetworkFilename(nnFileName);
            AppConfig::Get().saveAppConfig();
        }

        initLayerActivateType();
    }

    if (AppConfig::RunMode().isTrainMode()) {
        m_nn = std::make_shared<NeuralNetwork>(AppConfig::TrainingTopology());

        m_weightMatrixLenList = std::vector<int>(m_nn->getTopology().size() - 1);
        m_totalMutateLenght = 0;

        initWeightMatrixLenList(m_weightMatrixLenList);
        initLayerActivateType();
    }

    if (AppConfig::RunMode().isHumanMode()) {
        m_nn = nullptr;
    }
}
SnakeBrain::~SnakeBrain() {
}

SnakeDirection SnakeBrain::think(std::vector<double> &input) {

    m_nn->setInput(input);
    m_nn->feedForward();

    std::shared_ptr<std::vector<double>> outputLayerActivateValues = m_nn->activatedValVectorOfLayerAt(m_nn->getTopology().size() - 1);
    constexpr double lowest_double = std::numeric_limits<double>::lowest();

    double max = lowest_double;
    int maxIndex = -1;
    int outputSize = outputLayerActivateValues->size();

    for (int i = 0; i < outputSize; i++) {
        if (outputLayerActivateValues->at(i) >= max) {
            max = outputLayerActivateValues->at(i);
            maxIndex = i;
        }
    }

    double up = outputLayerActivateValues->at(0);
    double right = outputLayerActivateValues->at(1);
    double down = outputLayerActivateValues->at(2);
    double left = outputLayerActivateValues->at(3);

    if ((up == 0 && right == 0 && down == 0 && left == 0) || (up == 1 && right == 1 && down == 1 && left == 1)) {

        LOG(ERROR) << "Output layer value abnorml, return invalid direction";
        std::string outputLayerValueLog = fmt::format("outputLayerActivateValues = {}.", *outputLayerActivateValues);
        LOG(ERROR) << outputLayerValueLog;

        return SnakeDirection::invalid;
    }

    const int indexRangeLow = 0;
    const int indexRangeHigh = 3;
    if (maxIndex >= indexRangeLow && maxIndex <= indexRangeHigh) {
        switch (maxIndex) {
        case 0:
            return SnakeDirection::up;
        case 1:
            return SnakeDirection::right;
        case 2:
            return SnakeDirection::down;
        case 3:
            return SnakeDirection::left;

        default:
            LOG(ERROR) << "maxIndex is out of range, should never see this log.";
            std::string outputLayerValueLog = fmt::format("max = {}, outputLayerActivateValues = {}.", max, *outputLayerActivateValues);
            LOG(ERROR) << outputLayerValueLog;

            return SnakeDirection::invalid;
        }

    } else {

        LOG(ERROR) << "should never see this log, maxIndex value = " << maxIndex;
        std::string outputLayerValueLog = fmt::format("max = {}, outputLayerActivateValues = {}.", max, *outputLayerActivateValues);
        LOG(ERROR) << outputLayerValueLog;

        return SnakeDirection::invalid;
    }
}

void SnakeBrain::mutate(const std::vector<double> &mutateValueTable) {

    auto topology = m_nn->getTopology();
    int weightMatrixNum = topology.size() - 1;

    for (int counter = 0; counter < m_totalMutateLenght; counter++) {

        int random = utility::random::generateRandomNumber(0, m_totalMutateLenght - 1);

        std::shared_ptr<Matrix> pm = nullptr;
        int row, col;
        int low = 0;
        int high = 0;
        int matrixIndex = 0;
        for (matrixIndex = 0; matrixIndex < weightMatrixNum; matrixIndex++) {

            if (matrixIndex == 0) {
                low = 0;
                high = m_weightMatrixLenList[matrixIndex];
            } else {
                low = high;
                high += m_weightMatrixLenList[matrixIndex];
            }

            if (random >= low && random < high) {
                break;

            } else {
                continue;
            }
        }

        if (random >= low && random < high) {
            // find it.
            pm = m_nn->weightMatrixAt(matrixIndex);
            row = (random - low) / pm->getColNum();
            col = (random - low) % pm->getColNum();

            double originalValue = pm->getValue(row, col);
            int mutateValueIndex = utility::random::generateRandomNumber(0, mutateValueTable.size() - 1);
            double mutatedValue = originalValue + mutateValueTable[mutateValueIndex];

            pm->setValue(row, col, mutatedValue);
        } else {
            LOG(ERROR) << fmt::format("should never see this log\n");
            LOG(ERROR) << fmt::format("random = {}, low = {}, high = {}, matrixIndex = {}\n", random, low, high, matrixIndex);
        }
    }
}

void SnakeBrain::setPlayboardModel(std::shared_ptr<PlayboardModel> &playboard) {
    m_playboard = playboard;
}

void SnakeBrain::buildVisionVector(std::vector<double> &visionVector,
                                   const BlockPosition &head,
                                   [[maybe_unused]] const BlockPosition &apple,
                                   const int row, [[maybe_unused]] const int col) {

    int index = 0;
    for_each(SnakeBrain::visionChangeList.begin(), SnakeBrain::visionChangeList.end(), [this, &visionVector, &head, row, &index](const auto &v) {
        double wallValue = 0.0, bodyValue = 0.0, foodValue = 0.0;
        int wallDistance = -1, bodyDistance = -1, foodDistance = -1;

        int tempRow = head.row;
        int tempCol = head.col;

        int wallStep = 1;
        int foodStep = 0;
        int snakeStep = 0;

        auto p = m_playboard.lock();
        while (tempRow += v[0], tempCol += v[1], p->isWithinBound(BlockPosition{tempRow, tempCol})) {

            if (p->isSnake(tempRow, tempCol) && bodyDistance == -1) {
                bodyDistance = snakeStep;
            }

            if (p->isApple(tempRow, tempCol) && foodDistance == -1) {
                foodDistance = foodStep;
            }

            wallDistance = wallStep;

            wallStep++;
            foodStep++;
            snakeStep++;
        }

        if (wallDistance == -1) {
            wallDistance = 0;
        }

        wallValue = SnakeBrain::wallValue(wallDistance, row);
        bodyValue = SnakeBrain::bodyValue(bodyDistance, row);
        foodValue = SnakeBrain::foodValue(foodDistance, row);

        std::vector<double> d{wallValue, bodyValue, foodValue};
        visionVector.insert(visionVector.end(), d.begin(), d.end());

        index++;
    });
}

double SnakeBrain::wallValue(const double wallDistance, const int totalBlockSize) {
    return wallDistance / double(totalBlockSize - 1);
}

double SnakeBrain::bodyValue(const double bodyDistance, const int totalBlockSize) {

    if (-1 == bodyDistance) {
        //该方向上没有蛇身
        return 1.0;
    } else {
        return bodyDistance / double(totalBlockSize - 1.0);
    }
}

double SnakeBrain::foodValue(const double foodDistance, const int totalBlockSize) {
    if (-1 == foodDistance) {
        //该方向上没有苹果
        return 0.0;
    } else {
        return (totalBlockSize - foodDistance - 1.0) / (totalBlockSize - 1.0);
    }
}

SnakeDirection SnakeBrain::randomDirection() {
    int d = 0;
    while (d = utility::random::generateRandomNumber(-2, 2), d == 0)
        ;
    return SnakeDirection(d);
}

void SnakeBrain::initLayerActivateType() {
    int layerNum = m_nn->getTopology().size();
    for (int layerIndex = 0; layerIndex < layerNum; layerIndex++) {
        if (layerIndex == 0) {
            // input layer
            m_nn->setLayerActivateType(layerIndex, NN::ActivationType::none);
        } else if (layerIndex == (layerNum - 1)) {
            // output layer
            m_nn->setLayerActivateType(layerIndex, NN::ActivationType::sigmoid);
        } else {
            // hidden layer
            m_nn->setLayerActivateType(layerIndex, NN::ActivationType::relu);
        }
    }
}

void SnakeBrain::initWeightMatrixLenList(std::vector<int> &list) {
    list.clear();

    auto topology = this->m_nn->getTopology();
    int weightMatrixNum = topology.size() - 1;

    for (int matrixIndex = 0; matrixIndex < weightMatrixNum; matrixIndex++) {

        list.push_back(topology[matrixIndex] * topology[matrixIndex + 1]);

        this->m_totalMutateLenght += list[matrixIndex];
    }
}
