#include "TexturedRectangle.h"

#include <SDL2/SDL.h>
#include <SDL_pixels.h>
#include <glog/logging.h>
#include <string>

void TexturedRectangle::SetDefaults() {
    // hot pink
    m_fillColor = SDL_Color{255, 105, 180, 255};
    // green
    m_borderColor = SDL_Color{0, 255, 0, 255};
    m_gridColor = SDL_Color{64, 64, 64, 255};
    m_rectMode = RectangleMode::border;
    m_borderWidth = 1;
    m_isDrawGrid = false;
    m_gridSize = 10;

    m_rectangle.x = 0;
    m_rectangle.y = 0;
    m_rectangle.w = 0;
    m_rectangle.h = 0;
}

TexturedRectangle::TexturedRectangle() {
    m_texture = nullptr;
    SetDefaults();
}

TexturedRectangle::TexturedRectangle(SDL_Renderer *renderer) {
    SDL_Surface *retrieveSurface = nullptr;

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, retrieveSurface);
    setTexture(texture);

    SDL_FreeSurface(retrieveSurface);

    SetDefaults();
}

// Destructor
TexturedRectangle::~TexturedRectangle() {
    if (nullptr != m_texture) {
        SDL_DestroyTexture(m_texture);
        setTexture(nullptr);
    }
}

void TexturedRectangle::setPosition(int x, int y) {
    m_rectangle.x = x;
    m_rectangle.y = y;
}

void TexturedRectangle::setDimensions(int w, int h) {
    m_rectangle.w = w;
    m_rectangle.h = h;
}

void TexturedRectangle::setRect(SDL_Rect rect) {
    m_rectangle = rect;
}

void TexturedRectangle::setDrawGrid(bool grid) {
    m_isDrawGrid = grid;
}

void TexturedRectangle::setGridSize(int gridSize) {
    if ((0 >= gridSize) || 0 != (m_rectangle.w % gridSize)) {
        m_gridSize = 10;
    } else {
        m_gridSize = gridSize;
    }
}
SDL_Texture *TexturedRectangle::getTexture() { return m_texture; }
void TexturedRectangle::setTexture(SDL_Texture *texture) {
    // pass a texture object in
    // release the member object before use the new one
    if (nullptr != m_texture) {
        SDL_DestroyTexture(m_texture);
        m_texture = nullptr;
        VLOG(0) << SDL_GetError();
    }

    m_texture = texture;
}

void TexturedRectangle::update() {}

void TexturedRectangle::render(SDL_Renderer *renderer) {

    if (nullptr != m_texture) {
        int result = SDL_RenderCopy(renderer, m_texture, nullptr, &m_rectangle);
        if (result != 0) {
            LOG(INFO) << "SDL_RenderCopy error: " << SDL_GetError();
        }
    } else {
        // save the origin render color
        // put it back latter
        SDL_Color origin_color;
        SDL_GetRenderDrawColor(renderer, &origin_color.r, &origin_color.g,
                               &origin_color.b, &origin_color.a);

        if (m_rectMode == RectangleMode::border) {

            SDL_SetRenderDrawColor(renderer, m_borderColor.r, m_borderColor.g,
                                   m_borderColor.b, SDL_ALPHA_OPAQUE);
            SDL_RenderDrawRect(renderer, &m_rectangle);

        } else if (m_rectMode == RectangleMode::fill) {
            SDL_SetRenderDrawColor(renderer, m_fillColor.r, m_fillColor.g,
                                   m_fillColor.b, SDL_ALPHA_OPAQUE);
            SDL_RenderFillRect(renderer, &m_rectangle);

        } else if (m_rectMode ==
                   (RectangleMode::border | RectangleMode::fill)) {
            SDL_SetRenderDrawColor(renderer, m_borderColor.r, m_borderColor.g, m_borderColor.b, SDL_ALPHA_OPAQUE);

            SDL_Rect borderRect = SDL_Rect{m_rectangle.x + m_borderWidth,
                                           m_rectangle.y + m_borderWidth,
                                           m_rectangle.w - 2 * m_borderWidth,
                                           m_rectangle.h - 2 * m_borderWidth};
            SDL_RenderDrawRect(renderer, &borderRect);

            // fill rect should smaller than border
            SDL_Rect fillRect = SDL_Rect{borderRect.x + m_borderWidth,
                                         borderRect.y + m_borderWidth,
                                         borderRect.w - 2 * m_borderWidth,
                                         borderRect.h - 2 * m_borderWidth};

            SDL_SetRenderDrawColor(renderer, m_fillColor.r, m_fillColor.g, m_fillColor.b, SDL_ALPHA_OPAQUE);
            SDL_RenderFillRect(renderer, &fillRect);

        } else {
            LOG(INFO) << "TODO: more rectmode";
        }

        // recover the origin color
        SDL_SetRenderDrawColor(renderer, origin_color.r, origin_color.g,
                               origin_color.b, origin_color.a);
    }

    if (m_isDrawGrid) {

        SDL_Color origin_color;
        SDL_GetRenderDrawColor(renderer, &origin_color.r, &origin_color.g,
                               &origin_color.b, &origin_color.a);

        SDL_SetRenderDrawColor(renderer, m_gridColor.r, m_gridColor.g,
                               m_gridColor.b, SDL_ALPHA_OPAQUE);

        int gridRowCount = m_rectangle.h / m_gridSize - 1;
        int gridColCount = m_rectangle.w / m_gridSize - 1;

        int x1 = m_rectangle.x, y1 = 0;
        int x2 = m_rectangle.x + m_rectangle.w, y2 = 0;

        for (int i = 1; i <= gridRowCount; i++) {
            y1 = i * m_gridSize + m_rectangle.y;
            y2 = i * m_gridSize + m_rectangle.y;
            SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
        }

        x1 = 0, y1 = m_rectangle.y;
        x2 = 0, y2 = m_rectangle.y + m_rectangle.h;
        for (int i = 1; i <= gridColCount; i++) {
            x1 = i * m_gridSize + m_rectangle.x;
            x2 = i * m_gridSize + m_rectangle.x;
            SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
        }

        // recover the origin color
        SDL_SetRenderDrawColor(renderer, origin_color.r, origin_color.g,
                               origin_color.b, origin_color.a);
    }
}
