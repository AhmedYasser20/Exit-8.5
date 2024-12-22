#pragma once

#include "../ecs/component.hpp"

#include <glm/glm.hpp>

namespace our
{

    class lightComponent : public Component
    {
    public:
        // The color of the light
        glm::vec3 mLightColor = glm::vec3(1.0f, 1.0f, 1.0f); // white light color ;

        // type of the light
        enum class LightType
        {
            Directional,
            Point,
            Spot
        } mLightType = LightType::Directional;
        bool isEnabled = true;  // New member to track light state

        void toggle() {
            this->isEnabled = !isEnabled;
        }
        // spot light properties, angle of the light cone
        float innerConeAngle = glm::radians(30.0f);
        // spot light properties, falloff of the light cone
        float outerConeAngle = glm::radians(45.0f);
        // spot light & directional light properties, direction of the light
        glm::vec3 direction = glm::vec3(0.0f, -1.0f, 0.0f);
        // attenuation properties
        glm::vec3 attenuation = glm::vec3(1.0f, 0.09f, 0.032f);

        // ID of the component is "Light"
        static std::string getID() { return "Light"; }

        // Reads the data of the component from a json object
        void deserialize(const nlohmann::json &data) override;
    };

}