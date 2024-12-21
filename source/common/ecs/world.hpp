#pragma once

#include <unordered_set>
#include "entity.hpp"
#include <reactphysics3d/reactphysics3d.h>
namespace our {

    // This class holds a set of entities
    class World {
        std::unordered_set<Entity*> entities; // These are the entities held by this world
        std::unordered_set<Entity*> markedForRemoval; // These are the entities that are awaiting to be deleted
                                                      // when deleteMarkedEntities is called
        rp3d::PhysicsCommon physicsCommon; // Factory pattern for creating physics world objects , logging, and memory management
        rp3d::PhysicsWorld* physicsWorld = nullptr; // This is the physics world that will be used for physics simulation
    public:

        World() = default;

        // This will deserialize a json array of entities and add the new entities to the current world
        // If parent pointer is not null, the new entities will be have their parent set to that given pointer
        // If any of the entities has children, this function will be called recursively for these children
        void deserialize(const nlohmann::json& data, Entity* parent = nullptr);

        // This will deserialize a json object of physics world settings and create a physics world
        // The physics world will be used for physics simulation
        void deserialize_physics(const nlohmann::json& data);

        // This adds an entity to the entities set and returns a pointer to that entity
        // WARNING The entity is owned by this world so don't use "delete" to delete it, instead, call "markForRemoval"
        // to put it in the "markedForRemoval" set. The elements in the "markedForRemoval" set will be removed and
        // deleted when "deleteMarkedEntities" is called.
        Entity* add() {
            //TODO: (Req 8) Create a new entity, set its world member variable to this,
            // and don't forget to insert it in the suitable container.
             // Create a new entity
            Entity* newEntity = new Entity();
            newEntity->world = this;
            entities.insert(newEntity);
            return newEntity;
        }

        // This returns and immutable reference to the set of all entites in the world.
        const std::unordered_set<Entity*>& getEntities() {
            return entities;
        }

        // This marks an entity for removal by adding it to the "markedForRemoval" set.
        // The elements in the "markedForRemoval" set will be removed and deleted when "deleteMarkedEntities" is called.
        void markForRemoval(Entity* entity){
            //TODO: (Req 8) If the entity is in this world, add it to the "markedForRemoval" set.
            if (entities.find(entity) != entities.end()) {
                markedForRemoval.insert(entity);
            }
        }

        // This removes the elements in "markedForRemoval" from the "entities" set.
        // Then each of these elements are deleted.
        void deleteMarkedEntities(){
            //TODO: (Req 8) Remove and delete all the entities that have been marked for removal
            for (auto entity : markedForRemoval) {
                entities.erase(entity);
                delete entity;
            }
            // deleted all entities in marked for Removal , so pointers are dangling. need empty the set 
            markedForRemoval.clear(); 
        }

        //This deletes all entities in the world
        void clear() {
            //TODO: (Req 8) Delete all the entites and make sure that the containers are empty
            deleteMarkedEntities();
            for (auto entity : entities) {
                delete entity;
            }
            entities.clear();
        }

        rp3d::PhysicsWorld* getPhysicsWorld() {
            return physicsWorld;
        }
        
        rp3d::PhysicsCommon& getPhysicsCommon() {
            return physicsCommon;
        }

        //Since the world owns all of its entities, they should be deleted alongside it.
        ~World(){
            clear();
                        // Destroy the physics world if it exists
            if(physicsWorld) {
                physicsCommon.destroyPhysicsWorld(physicsWorld);
            }
        }

        // The world should not be copyable
        World(const World&) = delete;
        World &operator=(World const &) = delete;
    };

}