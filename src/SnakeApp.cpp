#include "SnakeApp.h"

#include "FPSTimer.h"
#include <glog/logging.h>

#include "AppConfig.h"
#include "SnakeModel.h"
#include <chrono>
#include <fmt/core.h>
#include <fmt/ranges.h>
#include <memory>
#include <string>
#include <thread>

using namespace std;

DECLARE_int64(mode);

SnakeApp::SnakeApp() {
    state = SnakeAppState(SnakeAppState::init);

    m_snake = nullptr;
    m_playboard = nullptr;
    m_infoboard = nullptr;

    initialize();
}

SnakeApp::~SnakeApp() {}

bool SnakeApp::initialize() {
    setDefaults();

    initModels();

    return true;
}

int SnakeApp::start() {

    runLoop();

    onCleanup();

    return 0;
}

void SnakeApp::setMaxFrameRate(int framerate) {
    m_maxFrameRate = framerate;
    m_frameDuration = 1000 / m_maxFrameRate;
}

void SnakeApp::setDefaults() {
    setMaxFrameRate(60);
    m_frameDuration = 1000 / m_maxFrameRate;
    running = true;
}

void SnakeApp::initModels() {

    m_playboard = std::make_shared<PlayboardModel>(AppConfig::PlayboardRowNum(), AppConfig::PlayboardColNum());
    m_snake = std::make_shared<SnakeModel>();

    m_snake->setPlayboardForBrain(m_playboard);

    if (AppConfig::RunMode().isHumanMode() || AppConfig::RunMode().isAIMode()) {
        // human mode or AI mode
        // init UI related models
        m_infoboard = std::make_shared<InfoboardModel>();
        m_UIManager = std::make_unique<UIManager>();
        m_fpsTimer = std::make_unique<FPSTimer>();

        // observer pattern setup
        m_infoboard->addObserver(*m_UIManager);
        m_playboard->addObserver(*m_UIManager);

        m_snake->addObserver(*m_infoboard);
        m_snake->addObserver(*m_UIManager);
        this->addObserver(*m_infoboard);

        m_UIManager->addObserver(*this);

        m_fpsTimer->setUpdateHandle([this](double fps) {
            std::string fpsStr = fmt::format("FPS: {:.3f}", fps);
            this->m_infoboard->setFPSStr(fpsStr);
        });
    }

    m_playboard->addObserver(*m_snake);
    m_snake->addObserver(*m_playboard);

    m_snake->initSnake();
    m_playboard->initPlayboard();
}

void SnakeApp::runLoop() {

    setState(SnakeAppState::running);

    if (AppConfig::RunMode().isHumanMode() || AppConfig::RunMode().isAIMode()) {
        m_snake->setLastMoveTime(std::chrono::high_resolution_clock::now());
        m_fpsTimer->reset();
    }

    while (running) {

        if (AppConfig::RunMode().isHumanMode() || AppConfig::RunMode().isAIMode()) {

            auto start = std::chrono::high_resolution_clock::now();
            onEvent();

            onUpdate();

            onRender();

            // Frame capping
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> elapsed = end - start;
            if (elapsed.count() < m_frameDuration) {
                auto frameCappingDelay = std::chrono::milliseconds(m_frameDuration) - elapsed;
                std::this_thread::sleep_for(frameCappingDelay);
            }
            m_fpsTimer->tick();
        } else {
            // train mode no event, no UI render, just update
            onUpdate();
        }
    }
}

void SnakeApp::onEvent() {  m_UIManager->onEvent(); }

void SnakeApp::onUpdate() {
    // update models
    m_playboard->update();

    if (AppConfig::RunMode().isHumanMode() || AppConfig::RunMode().isAIMode()) {
        m_infoboard->update();
    }

    if (m_snake->getState().isDie() && state.isRunning()) {
        /* game should end */
        if (AppConfig::RunMode().isHumanMode() || AppConfig::RunMode().isAIMode()) {
            m_snake->pause();
            setState(SnakeAppState::end);
        } else {
            setState(SnakeAppState::end);
            onExit();
        }
    }

    m_snake->update();

    if (AppConfig::RunMode().isHumanMode() || AppConfig::RunMode().isAIMode()) {
        m_UIManager->update();
    }
}

void SnakeApp::onRender() { m_UIManager->onRender(); }

void SnakeApp::onCleanup() {
    if (AppConfig::RunMode().isHumanMode() || AppConfig::RunMode().isAIMode()) {
        m_UIManager->onCleanup();
    }
}

void SnakeApp::onExit() { running = false; }

void SnakeApp::field_changed([[maybe_unused]] UIManager &source, const string &field_name) {
    if (field_name == "quit") {

        onExit();

    } else if (field_name == "down") {
        if (AppConfig::RunMode().isHumanMode()) { m_snake->setDirection(SnakeDirection::down); }
    } else if (field_name == "up") {
        if (AppConfig::RunMode().isHumanMode()) { m_snake->setDirection(SnakeDirection::up); }
    } else if (field_name == "right") {
        if (AppConfig::RunMode().isHumanMode()) { m_snake->setDirection(SnakeDirection::right); }
    } else if (field_name == "left") {
        if (AppConfig::RunMode().isHumanMode()) { m_snake->setDirection(SnakeDirection::left); }

    } else if (field_name == "onPKeyPressed") {

        if (state.isRunning()) {
            setState(SnakeAppState::pause);
            m_snake->pause();
        }

    } else if (field_name == "onQKeyPressed") {
        if (state.isEnd()) {
            onExit();
        }

    } else if (field_name == "onRKeyPressed") {
        if (state.isPause()) {
            setState(SnakeAppState::running);
            m_snake->resume();
        }

        if (state.isEnd()) {

            m_snake->reset();
            m_playboard->reset();
            m_UIManager->reset();

            m_playboard->placeApple();

            setState(SnakeAppState::running);
        }

    } else if (field_name == "onSpacePressed") {
        m_snake->setManualToggle(!(m_snake->getManualToggle()));

    } else {
        // not interest in these events
    }
}
