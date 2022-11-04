#include "EntityManager.h"

#include <glog/logging.h>

#include "Utility.h"

EntityManager::~EntityManager() {

    deleteAllEntities();
}

bool EntityManager::createEntity(SDL_Renderer *renderer, const std::string &id, EntityLayer layer) {

    std::lock_guard<std::mutex> lockGuard(m_lock);

    if (0 != m_entityMap.count(id)) {
        // key already exist
        return false;
    }

    bool inserted = false;
    std::shared_ptr<Entity> entity;

    if (0 != m_reuseQueue.size()) {

        entity = m_reuseQueue.front();

        if (nullptr == entity) {
        } else {
            entity->setEntityId(id);
            m_reuseQueue.pop_front();
        }

    } else {
        entity = std::make_shared<Entity>(id, renderer);
    }

    entity->setEntityLayer(layer);
    inserted = m_entityMap.insert(std::make_pair(id, entity)).second;

    if (!inserted) {
        return false;
    }

    return true;
}

std::shared_ptr<Entity> EntityManager::createEntity(SDL_Renderer *renderer, EntityLayer layer) {
    std::string id = uuid::generate_uuid_v4();

    bool result = createEntity(renderer, id, layer);
    if (result) {
        return m_entityMap[id];
    } else {
        LOG(ERROR) << "createEntity failed, return nullptr";
        return nullptr;
    }
}

std::shared_ptr<Entity> EntityManager::getEntity(const std::string &id) {
    std::lock_guard<std::mutex> lockGuard(m_lock);
    return (m_entityMap[id]);
}

// Remove the entity
void EntityManager::removeEntity(const std::string &id) {
    std::lock_guard<std::mutex> lockGuard(m_lock);

    auto search = m_entityMap.find(id);
    if (search != m_entityMap.end()) {

        search->second->setPosition(0, 0);
        search->second->setDimensions(0, 0);
        search->second->setEntityId("");

        m_reuseQueue.push_back(search->second);
        m_entityMap.erase(search);
    } else {
        LOG(ERROR) << "EntityManager::removeEntity entity not found, key = " << id;
    }
}

// Update all the entities
void EntityManager::updateAllEntity() {
    size_t index = 0;
    for (auto it = m_entityMap.begin(); it != m_entityMap.end(); it++) {
        if (m_entityMap[it->first] != nullptr) {
            m_entityMap[it->first]->update();
        } else {
        }
        index++;
    }
}

void EntityManager::renderAllEntity() {
    renderLayerEntity(EntityLayer::BACKGROUND);
    renderLayerEntity(EntityLayer::MIDDLEGROUND);
    renderLayerEntity(EntityLayer::FOREGROUND);
}

void EntityManager::renderLayerEntity(EntityLayer layer) {
    std::lock_guard<std::mutex> lockGuard(m_lock);
    size_t index = 0;
    for (auto it = m_entityMap.begin(); it != m_entityMap.end(); it++) {

        std::shared_ptr<Entity> entity = m_entityMap[it->first];

        if (entity != nullptr) {
            if (entity->getEntityLayer() == layer) {
                entity->render();
            }
        } else {
        }

        index++;
    }
}

// Delete all of the entities
void EntityManager::deleteAllEntities() {
    m_entityMap.erase(m_entityMap.begin(), m_entityMap.end());
    m_reuseQueue.erase(m_reuseQueue.begin(), m_reuseQueue.end());
}
