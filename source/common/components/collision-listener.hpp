// components/collision-listener.hpp
#pragma once

#include <reactphysics3d/reactphysics3d.h>
#include "../ecs/world.hpp"
#include "rigid.hpp"
#include "../application.hpp"

namespace our
{

    class CollisionEventListener : public rp3d::EventListener
    {
        World &world;

    public:
        CollisionEventListener(World &world) : world(world) {}

        void onContact(const rp3d::CollisionCallback::CallbackData &callbackData) override
        {
            for (int i = 0; i < callbackData.getNbContactPairs(); i++)
            {
                const rp3d::CollisionCallback::ContactPair &contactPair = callbackData.getContactPair(i);

                // Get entities involved in collision
                Entity *entityA = static_cast<Entity *>(contactPair.getBody1()->getUserData());
                Entity *entityB = static_cast<Entity *>(contactPair.getBody2()->getUserData());

                if (!entityA || !entityB)
                    continue;

                // If it's a collision start
                if (contactPair.getEventType() == rp3d::CollisionCallback::ContactPair::EventType::ContactStart)
                {
                    if ((entityA->name == "player" && entityB->name == "red_door") ||
                        (entityB->name == "player" && entityA->name == "red_door"))
                    {
                        std::cout << "Player hit red door!" << std::endl;
                        world.setDoor(2);
                        //  You can set a flag in World here
                    }
                    else if ((entityA->name == "player" && entityB->name == "green_door") ||
                             (entityB->name == "player" && entityA->name == "green_door"))
                    {
                        std::cout << "Player hit green door!" << std::endl;
                        world.setDoor(1);
                        // You can set a flag in World here
                    }
                }
            }
        }
    };

}