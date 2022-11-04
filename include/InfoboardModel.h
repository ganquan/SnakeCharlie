#pragma once
#include "Observer/Observer.h"
#include "TextLabel.h"

#include <atomic>
#include <memory>
#include <string>

class SnakeDirection;
class SnakeModel;
class SnakeApp;

class InfoboardModel : public Observer<SnakeModel>, public Observer<SnakeApp>, public Observable<InfoboardModel> {
public:
    InfoboardModel();
    ~InfoboardModel();

    void update();

    std::string getScoreStr() { return m_scoreStr; }
    void setScoreStr(std::string &scoreStr) {
        m_scoreStr = scoreStr;
        this->notify(*this, "scoreStr");
    }

    std::string getStepCountStr() { return m_stepCountStr; }
    void setStepCountStr(std::string &stepCountStr) {
        m_stepCountStr = stepCountStr;
        this->notify(*this, "stepCountStr");
    }

    std::string getRefreshTimeStr() { return m_refreshTimeStr; }
    void setRefreshTimeStr(std::string &refreshTimeStr) {
        m_refreshTimeStr = refreshTimeStr;
        this->notify(*this, "refreshTimeStr");
    }

    std::string getDirectionStr() { return m_directionStr; }
    void setDirectionStr(std::string &directionStr) {
        m_directionStr = directionStr;
        this->notify(*this, "directionStr");
    }

    std::string getGameStateStr() { return m_gameStateStr; }
    void setGameStateStr(std::string &gameState) {
        m_gameStateStr = gameState;
        this->notify(*this, "gameStateStr");
    }

    std::string getGameCommandStr() { return m_gameCommandStr; }
    void setGameCommandStr(const std::string &gameCommand) {
        m_gameCommandStr = gameCommand;
        this->notify(*this, "gameCommandStr");
    }

    void setFPSStr(const std::string &fpsStr) {
        m_FPSStr = fpsStr;
        this->notify(*this, "fpsStr");
    }

    std::string getFPSStr() { return m_FPSStr; }

    void setInfoStr(const std::string &info) {
        m_infoStr = info;
        this->notify(*this, "infoStr");
    }
    std::string getInfoStr() { return m_infoStr; }

    std::string getTopologyStr() { return m_topologyStr; }
    std::string getTrainedGenNumStr() { return m_trainedGenNumStr; }
    std::string getNextDirectionStr() { return m_nextDirectionStr; }

    std::string getUpValueStr() { return m_upStr; }
    std::string getDownValueStr() { return m_downStr; }
    std::string getLeftValueStr() { return m_leftStr; }
    std::string getRightValueStr() { return m_rightStr; }

    void field_changed(SnakeModel &source, const string &field_name);
    void field_changed(SnakeApp &source, const string &field_name);

private:

    std::string m_FPSStr;

    std::string m_scoreStr;
    std::string m_stepCountStr;
    std::string m_refreshTimeStr;
    std::string m_directionStr;

    std::string m_gameStateStr;
    std::string m_gameCommandStr; 
    std::string m_infoStr;

    // AI
    std::string m_topologyStr;
    std::string m_trainedGenNumStr;
    std::string m_nextDirectionStr;
    std::string m_upStr;
    std::string m_downStr;
    std::string m_leftStr;
    std::string m_rightStr;
};
