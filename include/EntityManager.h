#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <queue>
#include <mutex>

#include "Entity.h"
#include "EntityLayer.h"

class EntityManager {
 public:
    static EntityManager &getInstance() {
        static EntityManager instance;
        return instance;
    }

    bool createEntity(SDL_Renderer *renderer, const std::string& id, EntityLayer layer);
    std::shared_ptr<Entity> createEntity(SDL_Renderer *renderer, EntityLayer layer);

    std::shared_ptr<Entity> getEntity(const std::string &id);
    void removeEntity(const std::string &id);
    void updateAllEntity();
    void renderAllEntity();
    void deleteAllEntities();

   void renderLayerEntity(EntityLayer layer);
    // Singleton pattern delete these copy method
 public:
    EntityManager(EntityManager const &) = delete;
    void operator=(EntityManager const &) = delete;

 private:
    EntityManager() {}
    ~EntityManager();

 private:
    std::mutex m_lock;
    std::unordered_map<std::string, std::shared_ptr<Entity>> m_entityMap;
    std::deque<std::shared_ptr<Entity>> m_reuseQueue;
};
