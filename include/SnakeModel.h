#pragma once

#include <SDL2/SDL.h>
#include <glog/logging.h>

#include "AppConfig.h"
#include "BlockModel.h"
#include "Observer/Observer.h"
#include "SnakeBrain.h"
#include "SnakeDirection.h"
#include "SnakeState.h"
#include <chrono>
#include <deque>
#include <memory>
#include <string>

class SnakeBrain;
class SnakeBlock;
class PlayboardModel;
class AppConfig;
class SnakeApp;

class SnakeModel : public Observable<SnakeModel>,
                   public Observer<PlayboardModel> {

    friend PlayboardModel;
    friend SnakeApp;

public:
    SnakeModel();
    ~SnakeModel();

    void update();
    void initSnake();
    void setDefaultMoveInterval() { setMoveInterval(AppConfig::InitInterval()); }
    int getMoveInterval() { return m_moveInterval; }

public:
    void pause();
    void resume();
    void reset();

public:
    // AI
    long double getRank() { return m_rank; }
    void setRank(long double rank) { m_rank = rank; }
    void fitness();
    void setPlayboardForBrain(std::shared_ptr<PlayboardModel> &playboard);
    std::shared_ptr<SnakeBrain> getBrain() { return m_brain; }
    bool getManualToggle() { return m_playManuallyToggle; }
    void setManualToggle(bool flag) { m_playManuallyToggle = flag; }

public:
    // getter and setter
    int getScore() { return m_score; }

    int getTotalStepCount() { return m_totalStepCount; }
    void setDirection(SnakeDirection d) {

        if (AppConfig::RunMode().isHumanMode()) {
            // human play mode
            // should not move opposite the current direction
            // this means snake will bite itself imediately
            if (m_currDirection + d != 0) {
                m_currDirection = d;
            }

        } else {
            // in training mode and AI mode
            // assume this is a knowledge snake should learn
            // so set the direction directly

            m_currDirection = d;
        }

        this->notify(*this, "direction");
    }

    SnakeDirection getCurrentDirection() { return m_currDirection; }

    void setNextDirection(SnakeDirection next) {
        m_nextDirection = next;
        this->notify(*this, "nextDirection");
    }
    SnakeDirection getNextDirection() { return m_nextDirection; }

    BlockPosition getNextHeadPostion() { return m_nextHeadPosition; }
    void setNextHeadPostion(BlockPosition next) { m_nextHeadPosition = next; }

    SnakeState getState() { return m_state; }
    void setState(SnakeState s) {
        m_state = s;
        this->notify(*this, "state");
    }

    std::string getStateInfo() { return m_stateInfo; }
    void setStateInfo(const std::string &info) { m_stateInfo = info; }

    bool getCrossoverFlag() { return m_crossoverFlag; }
    void setCrossoverFlag(bool flag) { m_crossoverFlag = flag; }

    void setLastMoveTime(std::chrono::time_point<std::chrono::high_resolution_clock> timePoint) { m_lastMoveTime = timePoint; }
    std::chrono::time_point<std::chrono::high_resolution_clock> getLastMoveTime() { return m_lastMoveTime; }

private:
    void setMoveInterval(int interval);
    int getEatStepCount() { return m_eatStepCount; }
    void setEatStepCount(int count) { m_eatStepCount = count; }

    void setScore(int score) {
        m_score = score;
        this->notify(*this, "score");
    }

    void setTotalStepCount(int stepCount) {
        m_totalStepCount = stepCount;
        this->notify(*this, "stepCount");
    }

    void addHeadBlock();
    void removeTailBlock();

private:
    // what a snake should do
    void playStep();
    void move();
    void eat();
    void forward();
    void decreaseMoveInterval();

private:
    // AI
    void buildNeuralNetworkInputVector(std::vector<double> &input);
    SnakeDirection thinking();

private:
    // helper
    BlockPosition removedTailPosition() { return m_removedTailPostion; }
    void increaseScore() { setScore(getScore() + 1); }
    void increaseTotalStep() { setTotalStepCount(getTotalStepCount() + 1); }

private:
    // observer
    void field_changed(PlayboardModel &source, const string &field_name);

private:
    int m_score;
    int m_totalStepCount;
    int m_eatStepCount;
    int m_moveInterval;

    long double m_rank;
    bool m_playManuallyToggle;
    bool m_crossoverFlag;

    SnakeDirection m_currDirection;
    SnakeDirection m_nextDirection;

    SnakeState m_state;
    std::string m_stateInfo;

    BlockPosition m_nextHeadPosition;
    BlockPosition m_applePosition;
    BlockPosition m_removedTailPostion;

    std::deque<SnakeBlock> m_snakeBodyQueue;
    std::shared_ptr<SnakeBrain> m_brain;

    std::chrono::time_point<std::chrono::high_resolution_clock> m_lastMoveTime;
};
