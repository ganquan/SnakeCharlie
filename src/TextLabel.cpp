#include "TextLabel.h"

#include <glog/logging.h>

#include "AppConfig.h"
#include "Entity.h"
#include "EntityManager.h"
#include "SnakeApp.h"

bool TextLabel::s_ttfInitialized = false;

TextLabel::TextLabel(SDL_Renderer *renderer) {
    TextLabel("", renderer);
}

TextLabel::TextLabel(std::string name, SDL_Renderer *renderer) {

    if (!s_ttfInitialized && TTF_Init() == -1) {
        LOG(ERROR) << "Could not initailize SDL2_ttf, error: " << TTF_GetError();
    } else {
        VLOG(0) << "SDL2_ttf system ready to go!";
        // Initialize TTF system only once, even if
        // we have multipl Text objects.
        s_ttfInitialized = true;
    }

    // Load our font file and set the font size
    m_textFont = TTF_OpenFont(AppConfig::TextLabelFontPath().c_str(),
                              AppConfig::TextLabelFontSize());
    // Confirm that it was loaded
    if (m_textFont == nullptr) {
        LOG(ERROR) << "Could not load font";
    }

    if (name.length() == 0) {
        std::shared_ptr<Entity> entity =
            EntityManager::getInstance().createEntity(renderer, EntityLayer::FOREGROUND);
        m_entityId = entity->getEntityId();
    } else {
        bool isSuccess = EntityManager::getInstance().createEntity(renderer, name, EntityLayer::FOREGROUND);
        if (isSuccess) {
            std::shared_ptr<Entity> entity = EntityManager::getInstance().getEntity(name);
            m_entityId = entity->getEntityId();
        } else {
            LOG(ERROR) << "TextLabel create Entity error. name = " << name;
        }
    }

    m_rect = SDL_Rect{0, 0, 0, 0};
    m_textColor = SDL_Color{255, 255, 224, 255}; // light yellow
    m_bgColor = SDL_Color{0, 0, 0, 255};
    m_highlightColor = SDL_Color{173, 255, 47, 255}; // green yellow

    m_isHighlight = false;
    m_texture = nullptr;
}

TextLabel::~TextLabel() {
    LOG(INFO) << "TextLabel::~TextLabel, id = " << m_entityId;

    if (nullptr != m_texture) {
        SDL_DestroyTexture(m_texture);
        m_texture = nullptr;
    }

    if (nullptr != m_textFont) {
        // Close our font subsystem
        TTF_CloseFont(m_textFont);
        m_textFont = nullptr;
    }
}

void TextLabel::setText(std::string text) {

    if (m_text == text) {
        VLOG(0) << "TextLabel::setText, text won't change, no need recreate texture.";
        return;
    }

    m_prevText = m_text;
    m_text = text;
}

void TextLabel::createTextureForRender() {
    if (m_text == m_prevText) {
        return;
    }

    // Update the label size according to text automatically
    int w;
    int h;
    int result = TTF_SizeText(m_textFont, m_text.c_str(), &w, &h);
    if (-1 == result) {
        LOG(ERROR) << "TTF_SizeText failed. text = " << m_text;
        return;
    }

    // now update the m_rect
    m_rect.w = w;
    m_rect.h = h;

    std::shared_ptr<Entity> lableEntity = EntityManager::getInstance().getEntity(m_entityId);
    if (nullptr == lableEntity) {
        LOG(ERROR) << "TextLabel can not find entity, key = " << m_entityId;
        return;
    }

    SDL_Color color;
    if (m_isHighlight) {
        color = m_highlightColor;
    } else {
        color = m_textColor;
    }

    SDL_Surface *surface = TTF_RenderText_LCD(m_textFont, m_text.c_str(), color, m_bgColor);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(lableEntity->getRenderer(), surface);
    SDL_FreeSurface(surface);

    lableEntity->getTexturedRectangle().setTexture(texture);
    lableEntity->getTexturedRectangle().setRect(m_rect);
}

void TextLabel::setTextColor(SDL_Color textColor) { m_textColor = textColor; }
void TextLabel::setBgColor(SDL_Color bgColor) { m_bgColor = bgColor; }
void TextLabel::setHighlightColor(SDL_Color hlColor) { m_highlightColor = hlColor; }
void TextLabel::setHighlight(bool hl) { m_isHighlight = hl; }

void TextLabel::setRect(SDL_Rect rect) { m_rect = rect; }

int TextLabel::getMinHeight() {
    return TTF_FontHeight(m_textFont);
}
