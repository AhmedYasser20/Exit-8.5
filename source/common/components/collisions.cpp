#include "collisions.hpp"
#include "../ecs/entity.hpp"
#include "../deserialize-utils.hpp"

namespace our
{
    void CollisionsComponent::deserialize(const nlohmann::json &data)
    {
        if (!data.is_object())
            return;
        start = data.value("start", start);
        end = data.value("end", end);
    }

    bool CollisionsComponent::isColliding(const CollisionsComponent &other) const
    {
        // Simple collision detection logic (AABB)
        return (start.x <= other.end.x && end.x >= other.start.x) &&
               (start.y <= other.end.y && end.y >= other.start.y) &&
               (start.z <= other.end.z && end.z >= other.start.z);
    }

    glm::vec3 CollisionsComponent::getCollisionPoint(const CollisionsComponent &other) const
    {
        // Placeholder for collision point calculation
        return glm::vec3(0.0f);
    }

    glm::vec3 CollisionsComponent::getCollisionNormal(const CollisionsComponent &other) const
    {
        // Placeholder for collision normal calculation
        return glm::vec3(0.0f);
    }
}