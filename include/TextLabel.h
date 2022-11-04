#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <string>

class TextLabel {
public:
    TextLabel(SDL_Renderer *renderer);
    TextLabel(std::string name, SDL_Renderer *renderer);
    ~TextLabel();

    void setText(std::string text);
    void createTextureForRender();
    void setTextColor(SDL_Color textColor);
    void setBgColor(SDL_Color bgColor);
    void setHighlightColor(SDL_Color hlColor);
    void setHighlight(bool highlight);
    void setRect(SDL_Rect m_rect);
    SDL_Rect getRect() { return m_rect; }
    int getMinHeight();

private:
    std::string m_text;
    std::string m_prevText;

    SDL_Color m_textColor;
    SDL_Color m_bgColor;
    SDL_Color m_highlightColor;
    bool m_isHighlight;

    SDL_Rect m_rect;

    std::string m_entityId;

    TTF_Font *m_textFont;
    SDL_Texture *m_texture;

    // Purpose is to ensure that TTF library is only
    // initialized one time
    static bool s_ttfInitialized;
};
