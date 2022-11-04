#pragma once

#include <atomic>
#include <memory>
#include <string>

#include "FPSTimer.h"
#include "InfoboardModel.h"
#include "PlayboardModel.h"
#include "UIManager.h"

class SnakeAppState {
public:
    enum State : int {
        init = 0,
        running = 1,
        pause = 2,
        end = 3
    };

    SnakeAppState() = default;
    constexpr SnakeAppState(State astate) : state(astate) {}

    constexpr bool isRunning() const { return state == running; }
    constexpr bool isPause() const { return state == pause; }
    constexpr bool isEnd() const { return state == end; }

    std::string description() {
        switch (state) {
        case init:
            return "INIT";
        case running:
            return "RUNNING";
        case pause:
            return "PAUSE";
        case end:
            return "END";
        }
    }

private:
    State state;
};

class SnakeApp : public Observable<SnakeApp>,
                 public Observer<UIManager> {
public:
    SnakeApp();
    ~SnakeApp();

    int start();

private:
    // GameApp basic structure
    bool initialize();
    void setDefaults();
    void setMaxFrameRate(int framerate);

    void initModels();
    void runLoop();

    void onEvent();
    void onUpdate();
    void onRender();

    void onExit();
    void onCleanup();

    void field_changed(UIManager &source, const string &field_name);

public:
    std::shared_ptr<PlayboardModel> getPlayboardModel() { return m_playboard; }
    std::shared_ptr<InfoboardModel> getInfoboardModel() { return m_infoboard; }
    std::shared_ptr<SnakeModel> getSnakeModel() { return m_snake; }

    SnakeAppState getState() { return state; }
    void setState(SnakeAppState state) {
        this->state = state;
        this->notify(*this, "state");
    }

private:
    bool running;
    SnakeAppState state;

    // max framerate, fps
    Uint32 m_maxFrameRate;
    // millisecond for each frame
    Uint64 m_frameDuration;

private:
    std::shared_ptr<SnakeModel> m_snake;
    std::shared_ptr<PlayboardModel> m_playboard;
    std::shared_ptr<InfoboardModel> m_infoboard;

    std::unique_ptr<FPSTimer> m_fpsTimer;
    std::unique_ptr<UIManager> m_UIManager;
};
