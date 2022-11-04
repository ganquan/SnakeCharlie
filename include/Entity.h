#pragma once

#include <SDL2/SDL.h>

#include <string>

#include "EntityLayer.h"
#include "TexturedRectangle.h"

class Entity {
public:
    Entity();
    Entity(SDL_Renderer *renderer);
    Entity(std::string id, SDL_Renderer *renderer);

    ~Entity();

    void update();

    void render();

    TexturedRectangle &getTexturedRectangle();

    void setPosition(int x, int y);
    void setDimensions(int w, int h);
    void setRenderer(SDL_Renderer *renderer);
    SDL_Renderer *getRenderer();

    std::string &getEntityId();
    void setEntityId(std::string id);

    void setEntityLayer(EntityLayer layer);
    EntityLayer getEntityLayer();

private:
    TexturedRectangle *m_sprite;
    SDL_Renderer *m_renderer;
    std::string m_id;

    EntityLayer m_Layer;
};
