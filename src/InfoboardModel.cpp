#include "InfoboardModel.h"

#include <glog/logging.h>

#include <string>

#include "AppConfig.h"
#include "EntityManager.h"
#include "NeuralNetwork/NeuralNetwork.h"
#include "SnakeApp.h"
#include "SnakeModel.h"
#include <fmt/core.h>
#include <fmt/ranges.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

const std::string c_keyInfo_running = std::string("Key: 'Cmd + Q' - quit. 'p/P' - pause.");
const std::string c_keyInfo_pause = std::string("Key: 'Cmd + Q' - quit. 'r/R' - resume.");
const std::string c_keyInfo_end = std::string("Key: 'q/Q' - quit. 'r/R' - restart.");

InfoboardModel::InfoboardModel() {

    m_FPSStr = fmt::format("FPS: {}", "");

    m_scoreStr = fmt::format("Score: 0");
    m_stepCountStr = fmt::format("StepCount: 0");
    m_refreshTimeStr = fmt::format("MoveInterval: {} ms", AppConfig::InitInterval());
    m_directionStr = fmt::format("Direction: ");

    m_gameStateStr = fmt::format("Game State: ");
    m_gameCommandStr = fmt::format(c_keyInfo_running);

    m_infoStr = fmt::format("");

    m_topologyStr = fmt::format("Topology: {}", "");
    m_trainedGenNumStr = fmt::format("Generation: {}", "");
    m_nextDirectionStr = fmt::format("Predict: {}", "");
    m_upStr = fmt::format("Up = {}", "");
    m_downStr = fmt::format("Down = {}", "");
    m_leftStr = fmt::format("Left = {}", "");
    m_rightStr = fmt::format("Right = {}", "");
}

InfoboardModel::~InfoboardModel() {
}

void InfoboardModel::update() {
}

void InfoboardModel::field_changed(SnakeModel &source, const std::string &field_name) {
    if (field_name == "score") {

        std::string newScoreStr = fmt::format("Score: {}", source.getScore());
        setScoreStr(newScoreStr);

    } else if (field_name == "refreshTime") {

        std::string newRefreshTimeStr = fmt::format("MoveInterval: {} ms", source.getMoveInterval());
        setRefreshTimeStr(newRefreshTimeStr);

    } else if (field_name == "direction") {

        std::string newDirectionStr = fmt::format("Direction: {}", source.getCurrentDirection().toString());
        setDirectionStr(newDirectionStr);

    } else if (field_name == "stepCount") {

        std::string newStepCountStr = fmt::format("StepCount: {}", source.getTotalStepCount());
        setStepCountStr(newStepCountStr);

    } else if (field_name == "nextDirection") {

        if (AppConfig::RunMode().isAIMode()) {
            json description = source.getBrain()->getNeuralNetwork()->getDescription();
            std::vector<int> t = source.getBrain()->getNeuralNetwork()->getTopology();

            m_topologyStr = fmt::format("Topology: {}", t);
            m_trainedGenNumStr = fmt::format("Generation: {}", description["generation"].get<int>());
            m_nextDirectionStr = fmt::format("Predict: {}", source.getNextDirection().toString());

            int outputLayerIndex = source.getBrain()->getNeuralNetwork()->getTopology().size() - 1;
            std::shared_ptr<std::vector<double>> vec = source.getBrain()->getNeuralNetwork()->activatedValVectorOfLayerAt(outputLayerIndex);
            m_upStr = fmt::format("Up = {}", vec->at(0));
            m_rightStr = fmt::format("Right = {}", vec->at(1));
            m_downStr = fmt::format("Down = {}", vec->at(2));
            m_leftStr = fmt::format("Left = {}", vec->at(3));
        }
        this->notify(*this, "nextDirectionStr");

    } else if (field_name == "state") {
        setInfoStr(source.getStateInfo());
        this->notify(*this, "infoStr");
    } else {
        // not interest in these events
    }
}

void InfoboardModel::field_changed(SnakeApp &source, const std::string &field_name) {
    if (field_name == "state") {
        std::string newGameStateStr = fmt::format("Game State: {}", source.getState().description());
        setGameStateStr(newGameStateStr);

        if (source.getState().isRunning()) {
            setGameCommandStr(c_keyInfo_running);
        }

        if (source.getState().isPause()) {
            setGameCommandStr(c_keyInfo_pause);
        }

        if (source.getState().isEnd()) {
            setGameCommandStr(c_keyInfo_end);
        }

    } else {
        // not interest in these events
    }
}
