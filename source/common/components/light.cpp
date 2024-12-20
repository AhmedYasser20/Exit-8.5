#include "light.hpp"
#include "../ecs/entity.hpp"
#include "../deserialize-utils.hpp"

namespace our {
    void lightComponent::deserialize(const nlohmann::json& data){
        if(!data.is_object()) return;
        mLightColor = data.value("color", mLightColor);

        std::string lightType = data.value("lightType", "directional");

        if(lightType == "directional"){
            mLightType = LightType::Directional;
        } else if(lightType == "point"){
            mLightType = LightType::Point;
        } else if(lightType == "spot"){
            mLightType = LightType::Spot;
            // spot light properties, angle of the light cone
            innerCutoff = data.value("innerCutoff", 30.0f);
            // spot light properties, falloff of the light cone
            outerCutoff = data.value("outerCutoff", 45.0f);
        }
        // attenuation for spot light & directional light properties, direction of the light
        attenuation = data.value("attenuation", attenuation);
        // direction of the light for spot light & directional light
        direction = data.value("direction", direction);
    }
}