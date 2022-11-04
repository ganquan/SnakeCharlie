#include "SnakeModel.h"
#include <fmt/core.h>
#include <fmt/ranges.h>
#include <glog/logging.h>
#include <limits>
#include <memory>

#include "AppConfig.h"
#include "NeuralNetwork/NeuralNetwork.h"
#include "SnakeApp.h"
#include "SnakeBrain.h"
#include "Utility.h"

SnakeModel::SnakeModel() {
    setStateInfo(" ");
    setState(SnakeState::alive);
    setScore(0);
    setTotalStepCount(0);
    setEatStepCount(0);
    setDefaultMoveInterval();

    m_brain = std::make_shared<SnakeBrain>();

    m_playManuallyToggle = false;
    m_crossoverFlag = false;
}

SnakeModel::~SnakeModel() {
    m_snakeBodyQueue.clear();
}

void SnakeModel::initSnake() {

    auto row = 1, col = 1;
    int d = 0; // init with a invalid direction and find a real direction
    while (d = utility::random::generateRandomNumber(-2, 2), d == 0)
        ;

    int offset = int(AppConfig::PlayboardRowNum() * 0.3); /* make the snake not closer to the bound */
    row = utility::random::generateRandomNumber(offset, AppConfig::PlayboardRowNum() - offset);
    col = utility::random::generateRandomNumber(offset, AppConfig::PlayboardColNum() - offset);

    setDirection(SnakeDirection(d));
    setNextDirection(SnakeDirection::invalid);

    setNextHeadPostion(BlockPosition{row, col});

    addHeadBlock();

    // set score and interval
    setScore(0); // notify the observer
    setDefaultMoveInterval();
}

void SnakeModel::update() {

    if ((AppConfig::RunMode().isAIMode() || AppConfig::RunMode().isHumanMode()) && getState().isAlive()) {

        auto now = std::chrono::high_resolution_clock::now();
        auto elapse = now - m_lastMoveTime;
        auto delay = std::chrono::milliseconds(getMoveInterval());
        if (elapse < delay) {
            return;
        }

        if (AppConfig::RunMode().isHumanMode() && getState().isAlive()) {
            this->playStep();
        }

        if (AppConfig::RunMode().isAIMode() && getState().isAlive()) {

            if (AppConfig::PlayManually()) {

                if (getManualToggle()) {

                    if (SnakeDirection::invalid != getNextDirection().rawValue()) {
                        setDirection(getNextDirection());
                    }

                    this->playStep();

                    auto next = thinking();
                    setNextDirection(next);

                    setManualToggle(!getManualToggle());
                }

            } else {

                LOG(INFO) << fmt::format("AI playStep, currDirection = {}, predict = {}, apple = {{{}, {}}}, head = {{{}, {}}}",
                                         getCurrentDirection().toString(),
                                         getNextDirection().toString(),
                                         m_applePosition.row,
                                         m_applePosition.col,
                                         m_nextHeadPosition.row,
                                         m_nextHeadPosition.col);

                if (SnakeDirection::invalid != getNextDirection().rawValue()) {
                    setDirection(getNextDirection());
                }

                this->playStep();

                auto next = thinking();
                setNextDirection(next);
            }
        }

        setLastMoveTime(std::chrono::high_resolution_clock::now());
    }

    if (AppConfig::RunMode().isTrainMode() && getState().isAlive()) {
        this->playStep();

        int stuckedThreshold = 0;
        const int initSnakeLen = 1;
        const int strictThreshold = AppConfig::PlayboardSize() - initSnakeLen;

        if (AppConfig::StrictWander()) {
            stuckedThreshold = strictThreshold;
        } else {
            stuckedThreshold = AppConfig::WanderThreshold();
            if (stuckedThreshold < strictThreshold) {
                stuckedThreshold = strictThreshold;
            }
        }

        int totalStep = getTotalStepCount();
        int eatStep = getEatStepCount();
        if (totalStep - eatStep > stuckedThreshold) {
            setState(SnakeState::die);
            return;
        }

        {
            auto direction = thinking();
            this->setDirection(direction);
        }
    }
}

void SnakeModel::setMoveInterval(int interval) {
    m_moveInterval = interval;
    this->notify(*this, "refreshTime");
}

void SnakeModel::decreaseMoveInterval() {

    auto curr_refresh_time = std::max(AppConfig::InitInterval() - getScore() * AppConfig::IntervalStep(), /* speed up with score increase */
                                      AppConfig::MinInterval() /* min speed */);

    setMoveInterval(curr_refresh_time);
}

void SnakeModel::fitness() {

    long double d = 100000.0;
    long double rewardScoreAndExplore = m_score + 0.5 + (0.5 * (m_totalStepCount - (m_totalStepCount / (m_score + 1.0))) / (m_totalStepCount + (m_totalStepCount / (m_score + 1.0))));

#if 1
    // reward lessAvgStep make the snake less wander, just go strait for apple as much as possible.
    // these paramters is tuned for 10x10 playboard
    // need adjust if training snake in other size,
    double exponentStart = 1.9;
    double delta = m_score / 10 * 0.03;
    double exponent = exponentStart - delta;
    long double rewardLessAvgStep = (1.0 / (m_totalStepCount / (m_score + 1.0))) * pow(rewardScoreAndExplore + 0.5, exponent);
    m_rank = rewardScoreAndExplore + rewardLessAvgStep;
#else
    // simple and fast
    // but the totalsteps will be large.
    m_rank = rewardScoreAndExplore;
#endif
    m_rank *= d;

    long double max = std::numeric_limits<long double>::max();
    if (m_rank > max) {
        fmt::print("m_rank is big than max {}\n", max);
        m_rank = max;
    }
}

void SnakeModel::setPlayboardForBrain(std::shared_ptr<PlayboardModel> &playboard) {
    m_brain->setPlayboardModel(playboard);
}

void SnakeModel::pause() { setState(SnakeState::freeze); }
void SnakeModel::resume() { setState(SnakeState::alive); }

void SnakeModel::playStep() {
    if (m_state.isAlive()) {
        move();
    }

    this->notify(*this, "playStep");
}

void SnakeModel::move() {

    BlockPosition currHeadPos = m_snakeBodyQueue.front().getPosition();
    BlockPosition nextHeadPos = currHeadPos.nextPositionOfDirection(SnakeDirection::changeMap[getCurrentDirection().rawValue()]);

    setNextHeadPostion(nextHeadPos);

    this->notify(*this, "move");
}

void SnakeModel::addHeadBlock() {

    m_snakeBodyQueue.push_front(SnakeBlock(m_nextHeadPosition));
    this->notify(*this, "addHeadBlock");
}

void SnakeModel::removeTailBlock() {

    m_removedTailPostion = m_snakeBodyQueue.back().getPosition();
    m_snakeBodyQueue.pop_back();

    this->notify(*this, "removeTailBlock");
}

void SnakeModel::eat() {

    increaseScore();
    increaseTotalStep();
    setEatStepCount(getTotalStepCount());

    if (AppConfig::RunMode().isHumanMode() || AppConfig::RunMode().isAIMode()) {
        decreaseMoveInterval();
    }

    addHeadBlock();

    if (getScore() == (AppConfig::PlayboardSize() - 1)) {
        // perfect snake, R.I.P
        setStateInfo("Perfect Snake, R.I.P");
        setState(SnakeState::die);
        return;
    }

    this->notify(*this, "eat");
}

void SnakeModel::forward() {
    removeTailBlock();
    addHeadBlock();

    increaseTotalStep();

    this->notify(*this, "forward");
}

void SnakeModel::reset() {
    setScore(0);
    setTotalStepCount(0);
    setEatStepCount(0);
    setDefaultMoveInterval();

    m_removedTailPostion = BlockPosition{0, 0};
    m_snakeBodyQueue.clear();

    initSnake();
    setStateInfo(" ");
    setState(SnakeState::alive);
}

void SnakeModel::buildNeuralNetworkInputVector(std::vector<double> &input) {
    // head block
    BlockPosition headPosition = m_snakeBodyQueue.front().getPosition();
    const int visionSize = 24;

    m_brain->buildVisionVector(input, headPosition, m_applePosition, AppConfig::PlayboardRowNum(), AppConfig::PlayboardColNum());

    std::vector<double> direction = this->getCurrentDirection().toVector();
    input.insert(input.begin() + visionSize, direction.begin(), direction.end());
}

SnakeDirection SnakeModel::thinking() {

    const int visionSize = 24;
    const int directionSize = 4;
    const int inputSize = visionSize + directionSize;

    std::vector<double> input(inputSize);
    input.clear();

    this->buildNeuralNetworkInputVector(input);

    SnakeDirection predictDirection = m_brain->think(input);
    if (predictDirection != SnakeDirection::invalid) {
        return predictDirection;
    } else {
        LOG(ERROR) << "Predict direction invalid, keep forward. currDirection = " << getCurrentDirection().toString();
        return this->getCurrentDirection();
    }
}

void SnakeModel::field_changed(PlayboardModel &source, const string &field_name) {

    if (field_name == "placeApple") {

        m_applePosition = source.getApplePosition();

    } else if (field_name == "eraseApple") {

        m_applePosition.row = 0;
        m_applePosition.col = 0;

    } else {
        // not interest in these events
    }
}
