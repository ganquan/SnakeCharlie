#include "Entity.h"

#include <glog/logging.h>

#include "Utility.h"

Entity::Entity() {
    m_renderer = nullptr;
    m_sprite = nullptr;

    m_id = uuid::generate_uuid_v4();
    m_Layer = EntityLayer::FOREGROUND;
}

Entity::Entity(SDL_Renderer *renderer) {
    m_renderer = renderer;
    m_sprite = new TexturedRectangle();

    m_id = uuid::generate_uuid_v4();
    m_Layer = EntityLayer::FOREGROUND;
}

Entity::Entity(std::string id, SDL_Renderer *renderer) {
    m_renderer = renderer;
    m_sprite = new TexturedRectangle();

    m_id = id;
    m_Layer = EntityLayer::FOREGROUND;
}

Entity::~Entity() {
    if (m_sprite != nullptr) {
        delete m_sprite;
        m_sprite = nullptr;
    }
}

void Entity::update() {}

void Entity::render() {
    if (nullptr != m_sprite) {
        m_sprite->render(m_renderer);
    }
}

TexturedRectangle &Entity::getTexturedRectangle() { return *m_sprite; }

void Entity::setPosition(int x, int y) { m_sprite->setPosition(x, y); }
void Entity::setDimensions(int w, int h) { m_sprite->setDimensions(w, h); }

std::string &Entity::getEntityId() { return m_id; }

void Entity::setEntityId(std::string id) {
    m_id = id;
}

void Entity::setRenderer(SDL_Renderer *renderer) { m_renderer = renderer; }
SDL_Renderer *Entity::getRenderer() { return m_renderer; }

void Entity::setEntityLayer(EntityLayer layer) { m_Layer = layer; }
EntityLayer Entity::getEntityLayer() { return m_Layer; }
