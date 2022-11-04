#include "UIManager.h"

#include <algorithm>
#include <memory>
#include <string>

#include "AppConfig.h"
#include "EntityManager.h"
#include "SnakeApp.h"
#include "SnakeModel.h"
#include "TextLabel.h"
#include <glog/logging.h>
#include <vector>

UIManager::UIManager() {

    m_window = nullptr;
    m_screen = nullptr;
    m_renderer = nullptr;

    initGUI();

    drawBackground();
    drawPlayBoard();
    drawInfoBoard();
    drawSnake();
    drawApple();
}

UIManager::~UIManager() {

    if (nullptr != m_renderer) {
        SDL_DestroyRenderer(m_renderer);
        m_renderer = nullptr;
    }

    if (nullptr != m_screen) {
        SDL_FreeSurface(m_screen);
        m_screen = nullptr;
    }

    if (nullptr != m_window) {
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
    }
}

void UIManager::initGUI() {
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        LOG(ERROR) << "SDL_Init failed: " << SDL_GetError();
        return;
    }

    SDL_Rect r;
    if (SDL_GetDisplayUsableBounds(0, &r) != 0) {
        LOG(ERROR) << "SDL_GetDisplayBounds failed: " << SDL_GetError();
        return;
    }

    int window_w = AppConfig::GUIWindowWidth();
    int window_h = AppConfig::GUIWindowHeight();

    int win_x = (r.w - window_w) / 2;
    int win_y = (r.h - window_h) / 2;
    m_window = SDL_CreateWindow(AppConfig::GUIWindowTitle().c_str(), win_x, win_y, window_w, window_h, SDL_WINDOW_SHOWN);
    if (m_window == nullptr) {
        LOG(ERROR) << "SDL_CreateWindow failed: " << SDL_GetError();
        return;
    }

    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_SOFTWARE);
    if (nullptr == m_renderer) {
        LOG(ERROR) << "SDL_CreateRenderer failed: " << SDL_GetError();
        return;
    }

#if 0
    m_screen = SDL_GetWindowSurface(m_window);
    if (m_screen == nullptr) {
        LOG(ERROR) << "SDL_GetWindowSurface failed: " << SDL_GetError();
        return;
    }
#endif
}

SDL_Renderer *UIManager::getRenderer() { return m_renderer; }

void UIManager::onEvent() {
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
        if (event.type == SDL_QUIT) {
            LOG(INFO) << "SDL_QUIT Event received, now exit.";
            this->notify(*this, "quit");
        }

        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.scancode) {
            case SDL_SCANCODE_DOWN:
            case SDL_SCANCODE_S:
                this->notify(*this, "down");
                break;
            case SDL_SCANCODE_UP:
            case SDL_SCANCODE_W:
                this->notify(*this, "up");
                break;
            case SDL_SCANCODE_RIGHT:
            case SDL_SCANCODE_D:
                this->notify(*this, "right");
                break;
            case SDL_SCANCODE_LEFT:
            case SDL_SCANCODE_A:
                this->notify(*this, "left");
                break;

            case SDL_SCANCODE_P:
                this->notify(*this, "onPKeyPressed");
                break;

            case SDL_SCANCODE_Q:
                this->notify(*this, "onQKeyPressed");
                break;
            case SDL_SCANCODE_R:
                this->notify(*this, "onRKeyPressed");
                break;
            case SDL_SCANCODE_SPACE:
                this->notify(*this, "onSpacePressed");
                break;

            default:
                break;
            }
        }
    }
}

void UIManager::onCleanup() {
    SDL_Quit();

    SDL_DestroyRenderer(m_renderer);
    m_renderer = nullptr;

#if 0
    SDL_FreeSurface(m_screen);
    m_screen = nullptr;
#endif
    SDL_DestroyWindow(m_window);
    m_window = nullptr;
}

void UIManager::onRender() {

    // Clear and Draw the Screen
    // Gives us a clear "canvas"
    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(m_renderer);

    // refresh UI
    EntityManager::getInstance().renderAllEntity();

    // Finally show what we've drawn
    SDL_RenderPresent(m_renderer);
}

void UIManager::update() {
    m_FPSLabel->createTextureForRender();

    m_scoreLabel->createTextureForRender();
    m_stepCountLabel->createTextureForRender();
    m_refreshTimeLabel->createTextureForRender();
    m_nextDirectionLabel->createTextureForRender();

    m_gameStateLabel->createTextureForRender();
    m_keyInfoLabel->createTextureForRender();
    m_infoLabel->createTextureForRender();

    m_nnTopologyLabel->createTextureForRender();
    m_nnTrainedGenLabel->createTextureForRender();
    m_predictDirectionLabel->createTextureForRender();

    m_upValueLabel->createTextureForRender();
    m_downValueLabel->createTextureForRender();
    m_leftValueLabel->createTextureForRender();
    m_rightValueLabel->createTextureForRender();

    drawSnake();
}

void UIManager::reset() {

    while (0 != m_snakeBodyEntityQueue.size()) {
        std::shared_ptr<Entity> entity = m_snakeBodyEntityQueue.back();
        if (nullptr != entity) {
            std::string id = entity->getEntityId();

            m_snakeBodyEntityQueue.pop_back();
            EntityManager::getInstance().removeEntity(id);
        }
    }
}

void UIManager::drawBackground() {

    string windowBackgroundName = string{"windowBackground"};
    EntityManager::getInstance().createEntity(m_renderer, windowBackgroundName, EntityLayer::BACKGROUND);
    std::shared_ptr<Entity> windowBackground = EntityManager::getInstance().getEntity(windowBackgroundName);

    windowBackground->getTexturedRectangle().setPosition(0, 0);
    windowBackground->getTexturedRectangle().setDimensions(AppConfig::GUIWindowWidth(), AppConfig::GUIWindowHeight());
    windowBackground->getTexturedRectangle().setRectFillColor(m_winBgColor);
    windowBackground->getTexturedRectangle().setRectangleMode(RectangleMode::fill);
}

void UIManager::drawPlayBoard() {

    string playboard_name = string{"playboard"};
    EntityManager::getInstance().createEntity(m_renderer, playboard_name,
                                              EntityLayer::MIDDLEGROUND);

    std::shared_ptr<Entity> playboard = EntityManager::getInstance().getEntity(playboard_name);

    const int start_x = 0, start_y = 0;

    int playboard_x = start_x + AppConfig::Margin();
    int playboard_y = start_y + AppConfig::Margin();

    playboard->getTexturedRectangle().setPosition(playboard_x, playboard_y);
    playboard->getTexturedRectangle().setDimensions(AppConfig::PlayboardWidth(), AppConfig::PlayboardHeight());
    playboard->getTexturedRectangle().setRectBorderColor(m_boardBorderColor);
    playboard->getTexturedRectangle().setRectFillColor(m_boardFillColor);
    playboard->getTexturedRectangle().setGridSize(AppConfig::PlayboardGridSize());
    playboard->getTexturedRectangle().setDrawGrid(AppConfig::PlayboardDrawGrid());

    playboard->getTexturedRectangle().setRectangleMode(RectangleMode::mix);
}

void UIManager::drawInfoBoard() {
    string scoreboard_name = string{"scoreboard"};
    EntityManager::getInstance().createEntity(m_renderer, scoreboard_name, EntityLayer::MIDDLEGROUND);
    std::shared_ptr<Entity> scoreboard = EntityManager::getInstance().getEntity(scoreboard_name);

    const int padding = AppConfig::Padding();

    string playboard_name = string{"playboard"};
    std::shared_ptr<Entity> playboard = EntityManager::getInstance().getEntity(playboard_name);

    int playboard_x = playboard->getTexturedRectangle().getRectangle().x;
    int playboard_y = playboard->getTexturedRectangle().getRectangle().y;
    int playboard_width = playboard->getTexturedRectangle().getRectangle().w;
    [[maybe_unused]] int playboard_height = playboard->getTexturedRectangle().getRectangle().h;

    int scoreboard_x = playboard_x + playboard_width + padding;
    int scoreboard_y = playboard_y;

    scoreboard->getTexturedRectangle().setPosition(scoreboard_x, scoreboard_y);
    scoreboard->getTexturedRectangle().setDimensions(AppConfig::InfoboardWidth(), AppConfig::InfoboardHeight());
    scoreboard->getTexturedRectangle().setRectBorderColor(m_boardBorderColor);
    scoreboard->getTexturedRectangle().setRectFillColor(m_boardFillColor);
    scoreboard->getTexturedRectangle().setRectangleMode(RectangleMode::mix);

    int labelOffsetX = 5;
    int labelOffsetY = 5;

    typedef struct {
        std::shared_ptr<TextLabel> &label;
        std::string id;
        int offsetX;
        int offsetY;

    } LabelConfig;

    std::vector<LabelConfig> labelConf{

        LabelConfig{m_FPSLabel, "m_FPSLabel", 0, 5},

        LabelConfig{m_scoreLabel, "m_scoreLabel", 0, 20},
        LabelConfig{m_stepCountLabel, "m_stepCountLabel", 0, 5},
        LabelConfig{m_refreshTimeLabel, "refreshTimeLabel", 0, 5},

        LabelConfig{m_nextDirectionLabel, "m_nextDirectionLabel", 0, 20},
        LabelConfig{m_gameStateLabel, "m_gameStateLabel", 0, 5},
        LabelConfig{m_keyInfoLabel, "m_keyInfoLabel", 0, 5},

        LabelConfig{m_infoLabel, "m_infoLabel", 0, 20},
        LabelConfig{m_nnTopologyLabel, "m_nnTopologyLabel", 0, 5},
        LabelConfig{m_nnTrainedGenLabel, "m_nnTrainedGenLabel", 0, 5},
        LabelConfig{m_predictDirectionLabel, "m_predictDirectionLabel", 0, 5},

        LabelConfig{m_upValueLabel, "m_upValueLabel", 0, 20},
        LabelConfig{m_downValueLabel, "m_downValueLabel", 0, 5},
        LabelConfig{m_leftValueLabel, "m_leftValueLabel", 0, 5},
        LabelConfig{m_rightValueLabel, "m_rightValueLabel", 0, 5}

    };

    // layout
    for (unsigned long i = 0; i < labelConf.size(); i++) {

        int lableLayoutStartX = 0;
        int lableLayoutStartY = 0;

        if (0 == i) {
            lableLayoutStartX = scoreboard_x;
            lableLayoutStartY = scoreboard_y;

            lableLayoutStartX += labelOffsetX;
            lableLayoutStartY += labelOffsetY;

        } else {
            lableLayoutStartX = labelConf[i - 1].label->getRect().x + labelConf[i - 1].label->getRect().w;

            lableLayoutStartY = labelConf[i - 1].label->getRect().y + labelConf[i - 1].label->getRect().h;
        }

        labelConf[i].label = std::make_shared<TextLabel>(labelConf[i].id, m_renderer);
        labelConf[i].label->setRect(SDL_Rect{lableLayoutStartX + labelConf[i].offsetX,
                                             lableLayoutStartY + labelConf[i].offsetY, 0,
                                             labelConf[i].label->getMinHeight()});
    }

    // textColor
    m_scoreLabel->setTextColor(m_scoreColor);
    m_infoLabel->setTextColor(m_boardBorderColor);
    m_FPSLabel->setTextColor(m_fpsColor);
}

void UIManager::drawSnake() {
    if (0 == m_snakeBodyEntityQueue.size()) {
        return;
    }

    if (1 == m_snakeBodyEntityQueue.size()) {
        m_snakeBodyEntityQueue.front()->getTexturedRectangle().setRectFillColor(m_snakeHeadColor);
    }

    if (m_snakeBodyEntityQueue.size() > 1) {
        auto oldHeadEntity = m_snakeBodyEntityQueue[1];

        oldHeadEntity->getTexturedRectangle().setRectFillColor(m_snakeBodyColor);
        m_snakeBodyEntityQueue.front()->getTexturedRectangle().setRectFillColor(m_snakeHeadColor);
    }
}

void UIManager::drawApple() {
    // create apple entity
    std::string apple_name = std::string("apple");
    EntityManager::getInstance().createEntity(m_renderer, apple_name, EntityLayer::FOREGROUND);
    m_appleEntity = EntityManager::getInstance().getEntity(apple_name);

    m_appleEntity->getTexturedRectangle().setRectFillColor(m_appleColor); // red
    m_appleEntity->getTexturedRectangle().setRectangleMode(RectangleMode::fill);
}

void UIManager::field_changed(InfoboardModel &source, const string &field_name) {
    if (field_name == "scoreStr") {
        m_scoreLabel->setText(source.getScoreStr());
    } else if (field_name == "stepCountStr") {
        m_stepCountLabel->setText(source.getStepCountStr());
    } else if (field_name == "refreshTimeStr") {
        m_refreshTimeLabel->setText(source.getRefreshTimeStr());
    } else if (field_name == "directionStr") {
        m_nextDirectionLabel->setText(source.getDirectionStr());
    } else if (field_name == "gameStateStr") {
        m_gameStateLabel->setText(source.getGameStateStr());

    } else if (field_name == "gameCommandStr") {
        m_keyInfoLabel->setText(source.getGameCommandStr());
    } else if (field_name == "infoStr") {

        m_infoLabel->setText(source.getInfoStr());

    } else if (field_name == "nextDirectionStr") {

        if (AppConfig::RunMode().isAIMode()) {

            m_nnTopologyLabel->setText(source.getTopologyStr());
            m_nnTrainedGenLabel->setText(source.getTrainedGenNumStr());
            m_predictDirectionLabel->setText(source.getNextDirectionStr());

            m_upValueLabel->setText(source.getUpValueStr());
            m_downValueLabel->setText(source.getDownValueStr());
            m_leftValueLabel->setText(source.getLeftValueStr());
            m_rightValueLabel->setText(source.getRightValueStr());
        }

    } else if (field_name == "fpsStr") {
        m_FPSLabel->setText(source.getFPSStr());
    } else {
    }
}

void UIManager::field_changed(PlayboardModel &source, const string &field_name) {

    if (field_name == "placeApple") {

        int apple_x = source.getApplePosition().col * AppConfig::SnakeBlockWidth() + AppConfig::Margin();
        int apple_y = source.getApplePosition().row * AppConfig::SnakeBlockHeight() + AppConfig::Margin();

        m_appleEntity->setPosition(apple_x, apple_y);
        m_appleEntity->setDimensions(AppConfig::SnakeBlockWidth(), AppConfig::SnakeBlockHeight());

    } else if (field_name == "eraseApple") {

        m_appleEntity->setDimensions(0, 0);

    } else {
    }
}

void UIManager::field_changed(SnakeModel &source, const string &field_name) {

    if (field_name == "addHeadBlock") {

        std::shared_ptr<Entity> piece_entity =
            EntityManager::getInstance().createEntity(m_renderer, EntityLayer::FOREGROUND);

        int snake_x = source.getNextHeadPostion().col * AppConfig::SnakeBlockWidth() + AppConfig::Margin();
        int snake_y = source.getNextHeadPostion().row * AppConfig::SnakeBlockHeight() + AppConfig::Margin();

        piece_entity->getTexturedRectangle().setPosition(snake_x, snake_y);
        piece_entity->getTexturedRectangle().setDimensions(AppConfig::SnakeBlockWidth(),
                                                           AppConfig::SnakeBlockHeight());

        piece_entity->getTexturedRectangle().setRectBorderColor(m_snakeBodyBorderColor);
        piece_entity->getTexturedRectangle().setRectangleMode(RectangleMode::mix);

        m_snakeBodyEntityQueue.push_front(piece_entity);

    } else if (field_name == "removeTailBlock") {

        if (m_snakeBodyEntityQueue.size() > 0) {

            std::shared_ptr<Entity> entity = m_snakeBodyEntityQueue.back();

            if (nullptr != entity) {
                std::string id = entity->getEntityId();

                m_snakeBodyEntityQueue.pop_back();
                EntityManager::getInstance().removeEntity(id);
            }
        }

    } else if (field_name == "nextDirection") {

        if (AppConfig::RunMode().isAIMode()) {
            std::vector<std::shared_ptr<TextLabel>> vec{
                m_upValueLabel,
                m_rightValueLabel,
                m_downValueLabel,
                m_leftValueLabel};

            for (auto &l : vec) {
                l->setHighlight(false);
            }

            int highlightIndex = source.getNextDirection().toVectorIndex();
            if (highlightIndex != -1) {
                vec[highlightIndex]->setHighlight(true);
            }

            if (source.getCurrentDirection() != source.getNextDirection()) {
                m_predictDirectionLabel->setHighlightColor(m_snakeHeadColor);
                m_predictDirectionLabel->setHighlight(true);
            } else {
                m_predictDirectionLabel->setHighlight(false);
            }
        }

    } else {
        // not interesting event
    }
}
