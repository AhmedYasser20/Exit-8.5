#include "world.hpp"
#include <json/json.hpp>
#include "../deserialize-utils.hpp"

namespace our {

    // This will deserialize a json array of entities and add the new entities to the current world
    // If parent pointer is not null, the new entities will be have their parent set to that given pointer
    // If any of the entities has children, this function will be called recursively for these children
    void World::deserialize(const nlohmann::json& data, Entity* parent){
        if(!data.is_array()) return;
        for(const auto& entityData : data){
            //TODO: (Req 8) Create an entity, make its parent "parent" and call its deserialize with "entityData".
            Entity * newEntity= add(); 
            newEntity->parent = parent; // if parent is null , idicates entity parent is root "world"
            newEntity->deserialize(entityData);
            if(entityData.contains("children")){
                //TODO: (Req 8) Recursively call this world's "deserialize" using the children data
                // and the current entity as the parent
                deserialize(entityData["children"], newEntity);
            }
        }
    }

        void World::deserialize_physics(const nlohmann::json& data){
        if(!data.is_object()) return;
        rp3d::PhysicsWorld::WorldSettings settings;
        glm::vec3 gravity(settings.gravity.x, settings.gravity.y, settings.gravity.z);
        gravity = data.value("gravity", gravity);
        settings.gravity = rp3d::Vector3(gravity.x, gravity.y, gravity.z);
        settings.worldName = data.value("worldName", settings.worldName);
        // TODO: Support world settings like gravity and world name
        this->physicsWorld = this->physicsCommon.createPhysicsWorld(settings);
    }

}