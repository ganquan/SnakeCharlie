#pragma once

#include <SDL2/SDL.h>

#include <string>

enum RectangleMode : Uint8 { border = 0x01,
                             fill = 0x10,
                             mix = 0x11 };

class TexturedRectangle {
public:
    TexturedRectangle();
    TexturedRectangle(SDL_Renderer *renderer);
    ~TexturedRectangle();

    void setPosition(int x, int y);
    void setDimensions(int w, int h);
    void setRect(SDL_Rect rect);
    void setDrawGrid(bool grid);
    void setGridSize(int gridSize);

    SDL_Texture *getTexture();
    void setTexture(SDL_Texture *texture);

    // Update every frame
    void update();
    // Render
    void render(SDL_Renderer *renderer);

public:
    inline SDL_Rect getRectangle() const { return m_rectangle; }
    inline void setRectBorderColor(SDL_Color color) { m_borderColor = color; }
    inline void setGridColor(SDL_Color color) { m_gridColor = color; }
    inline void setRectFillColor(SDL_Color color) { m_fillColor = color; }

    inline SDL_Color getRectBorderColor() const { return m_borderColor; }
    inline SDL_Color getRectFillColor() const { return m_fillColor; }

    inline void setRectangleMode(RectangleMode mode) { m_rectMode = mode; }
    inline RectangleMode getRectangleMode() const { return m_rectMode; }

    // TODO: add border width support
    // not available now
    //  inline void SetBorderWidth(uint8_t w) { m_borderWidth = w; }
    //  inline uint8_t GetBorderWidth() const { return m_borderWidth; }
private:
    void SetDefaults();

    SDL_Rect m_rectangle;
    RectangleMode m_rectMode;

    bool m_isDrawGrid;
    int m_gridSize;

    SDL_Color m_borderColor;
    SDL_Color m_fillColor;
    SDL_Color m_gridColor;

    // border line width, in pixel
    uint8_t m_borderWidth;

    SDL_Texture *m_texture;
};
