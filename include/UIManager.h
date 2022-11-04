#pragma once
#include "Observer/Observer.h"
#include <SDL2/SDL.h>
#include <deque>
#include <memory>

class InfoboardModel;
class PlayboardModel;
class SnakeModel;
class TextLabel;
class Entity;

class UIManager : public Observable<UIManager>,
                  public Observer<InfoboardModel>,
                  public Observer<PlayboardModel>,
                  public Observer<SnakeModel> {
public:
    UIManager();
    ~UIManager();

    void initGUI();
    void onEvent();
    void onCleanup();
    void onRender();
    void update();
    void reset();

    void field_changed(InfoboardModel &source, const string &field_name);
    void field_changed(PlayboardModel &source, const string &field_name);
    void field_changed(SnakeModel &source, const string &field_name);

public:
    SDL_Renderer *getRenderer();

private:
    void drawBackground();
    void drawPlayBoard();
    void drawInfoBoard();
    void drawSnake();
    void drawApple();

private:
    std::shared_ptr<TextLabel> m_FPSLabel;

    std::shared_ptr<TextLabel> m_scoreLabel;
    std::shared_ptr<TextLabel> m_stepCountLabel;
    std::shared_ptr<TextLabel> m_refreshTimeLabel;
    std::shared_ptr<TextLabel> m_nextDirectionLabel;

    std::shared_ptr<TextLabel> m_gameStateLabel;
    std::shared_ptr<TextLabel> m_keyInfoLabel;

    std::shared_ptr<TextLabel> m_nnTopologyLabel;
    std::shared_ptr<TextLabel> m_nnTrainedGenLabel;
    std::shared_ptr<TextLabel> m_predictDirectionLabel;
    std::shared_ptr<TextLabel> m_upValueLabel;
    std::shared_ptr<TextLabel> m_downValueLabel;
    std::shared_ptr<TextLabel> m_leftValueLabel;
    std::shared_ptr<TextLabel> m_rightValueLabel;

    std::shared_ptr<TextLabel> m_infoLabel;

    std::shared_ptr<Entity> m_appleEntity;

    std::deque<std::shared_ptr<Entity>> m_snakeBodyEntityQueue;

private:
    SDL_Window *m_window;
    SDL_Surface *m_screen;
    SDL_Renderer *m_renderer;

private:
    // colors
    SDL_Color m_winBgColor{0, 0, 0, 255};            // window background color black
    SDL_Color m_boardBorderColor{251, 139, 30, 255}; // orange
    SDL_Color m_boardFillColor{0, 0, 0, 255};

    SDL_Color m_snakeHeadColor{65, 105, 225, 255}; // blue
    SDL_Color m_snakeBodyColor{50, 128, 50, 255};  // green
    SDL_Color m_snakeBodyBorderColor{107, 142, 35, 255};

    SDL_Color m_appleColor{205, 67, 61, 255}; // red
    SDL_Color m_fpsColor{127, 255, 0, 255};   // green - chartreuse
    SDL_Color m_scoreColor{255, 69, 0, 255};  // red - orange red
};
