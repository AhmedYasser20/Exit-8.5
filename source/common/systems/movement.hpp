#pragma once

#include "../ecs/world.hpp"
#include "../components/movement.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include "../components/rigid.hpp"
#include "../components/free-camera-controller.hpp"

namespace our
{

    // The movement system is responsible for moving every entity which contains a MovementComponent.
    // This system is added as a simple example for how use the ECS framework to implement logic. 
    // For more information, see "common/components/movement.hpp"
    class MovementSystem {
    public:

        // This should be called every frame to update all entities containing a MovementComponent. 
        void update(World* world, float deltaTime) {
            rp3d::PhysicsWorld* physicsWorld = world->getPhysicsWorld();
            if(!physicsWorld) return;
            // Constant physics time step (60 FPS)
            const float timeStep = 1.0f / 60.0f;
            float delta = deltaTime;
            while(delta > 0.0f){
                // If the remaining time is smaller than the time step, use the remaining time
                float step = glm::min(delta, timeStep);
                // Update the physics world
                physicsWorld->update(step);
                // Subtract the time step from the remaining time
                delta -= step;
            }
            // For each entity in the world
            for(auto entity : world->getEntities()){
                // Get the movement component if it exists
                MovementComponent* movement = entity->getComponent<MovementComponent>();
                // If the movement component exists
                if(movement){
                    // Change the position and rotation based on the linear & angular velocity and delta time.
                    const rp3d::Vector3& pos = entity->localTransform.getPosition();
                    const rp3d::Quaternion& rot = entity->localTransform.getRotation();
                    glm::vec3 position(pos.x, pos.y, pos.z);
                    glm::quat rotation((float)rot.w, (float)rot.x, (float)rot.y, (float)rot.z);
                    position += deltaTime * movement->linearVelocity;
                    rotation = rotation + 0.5f * deltaTime * glm::quat(0, movement->angularVelocity.x, movement->angularVelocity.y, movement->angularVelocity.z) * rotation;
                    rotation = glm::normalize(rotation);
                    entity->localTransform.setPosition(position);
                    entity->localTransform.setRotation(rotation);
                }
                RigidBodyComponent* rgb = entity->getComponent<RigidBodyComponent>();
                if(rgb){
                    FreeCameraControllerComponent* fcc = entity->getComponent<FreeCameraControllerComponent>();
                    if(fcc){
                        rp3d::Transform transform = rgb->getBody()->getTransform();
                        // orientation stays the same
                        transform.setOrientation(entity->localTransform.getRotation());
                        entity->localTransform.setTransform(transform);
                    }else{
                        entity->localTransform.setTransform(rgb->getBody()->getTransform());
                    }
                }
            }
        }
    };
}
