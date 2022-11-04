#include "PlayboardModel.h"

#include <glog/logging.h>

#include <string>

#include "AppConfig.h"
#include "EntityManager.h"
#include "SnakeApp.h"
#include "SnakeModel.h"

using namespace std;

PlayboardModel::PlayboardModel() {
    m_row = 10;
    m_col = 10;

    m_applePosition.row = -1;
    m_applePosition.col = -1;

    PlayboardModel(m_row, m_col);
}

PlayboardModel::PlayboardModel(int row, int col) {
    m_row = row;
    m_col = col;

    m_applePosition.row = -1;
    m_applePosition.col = -1;

    m_board = new char *[m_row];
    for (auto i = 0; i < m_row; i++) {
        m_board[i] = new char[m_col];
        memset(m_board[i], DEFAULT_EMPTY, m_col);
    }
}

PlayboardModel::~PlayboardModel() {
    for (auto i = 0; i < m_row; i++) {
        delete m_board[i];
    }

    delete m_board;
}

void PlayboardModel::initPlayboard() {
    placeApple();
}

void PlayboardModel::reset() {
    for (auto i = 0; i < m_row; i++) {
        memset(m_board[i], DEFAULT_EMPTY, m_col);
    }

    m_applePosition.row = -1;
    m_applePosition.col = -1;

    // tell observer earase apple event
    eraseApple();
}

void PlayboardModel::update() {}

void PlayboardModel::eraseApple() {
    // after eraseApple, set the apple pos to invalid
    m_applePosition.row = -1;
    m_applePosition.col = -1;

    this->notify(*this, "eraseApple");
}

void PlayboardModel::placeApple() {

    // before placeApple, set the apple pos to invalid
    m_applePosition.row = -1;
    m_applePosition.col = -1;
    getRandomEmptyPosition(m_applePosition.row, m_applePosition.col);

    this->addBlock(AppleBlock(m_applePosition));

    this->notify(*this, "placeApple");
}

void PlayboardModel::field_changed(SnakeModel &source, const string &field_name) {

    if (field_name == "move") {

        bool isInBound = isWithinBound(source.getNextHeadPostion());

        if (!isInBound) {
            source.setStateInfo("Snake hit the wall.");
            source.setState(SnakeState::die);
            return;
        }

        if (this->isApple(source.getNextHeadPostion().row, source.getNextHeadPostion().col)) {
            source.eat();
        } else if (this->isEmpty(source.getNextHeadPostion().row, source.getNextHeadPostion().col)) {
            source.forward();
        } else if (this->isSnake(source.getNextHeadPostion().row, source.getNextHeadPostion().col)) {
            if (AppConfig::RunMode().isHumanMode() || AppConfig::RunMode().isAIMode()) {
                LOG(INFO) << "Snake bite itself.";
            }
            source.setStateInfo("Snake bite itself.");
            source.setState(SnakeState::die);
        } else {
            source.setState(SnakeState::die);
        }

    } else if (field_name == "addHeadBlock") {

        this->addBlock(SnakeBlock(source.getNextHeadPostion()));

    } else if (field_name == "removeTailBlock") {

        this->addBlock(EmptyBlock(source.removedTailPosition()));

    } else if (field_name == "eat") {

        eraseApple();
        placeApple();

    } else {
        // not interest in these events
    }
}
