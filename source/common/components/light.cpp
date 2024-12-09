#include "light.hpp"
#include "../ecs/entity.hpp"
#include "../deserialize-utils.hpp"

namespace our {
    void lightComponent::deserialize(const nlohmann::json& data){
        if(!data.is_object()) return;
        mLightColor = data.value("color", glm::vec3(1.0f, 1.0f, 1.0f));
    }
}