#pragma once

#include "../ecs/component.hpp"
#include <glm/glm.hpp>

namespace our
{

    class CollisionsComponent : public Component
    {
    public:
        glm::vec3 start;
        glm::vec3 end;

        // The ID of this component type is "Collisions"
        static std::string getID() { return "Collisions"; }

        // Reads camera parameters from the given json object
        void deserialize(const nlohmann::json &data) override;

        // Creates and returns the camera view matrix
        glm::vec3 getStart() const { return start; }
        glm::vec3 getEnd() const { return end; }

        bool isColliding(const CollisionsComponent &other) const;

        glm::vec3 getCollisionPoint(const CollisionsComponent &other) const;

        glm::vec3 getCollisionNormal(const CollisionsComponent &other) const;
    };
}