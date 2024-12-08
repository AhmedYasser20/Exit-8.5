#pragma once

#include "../ecs/component.hpp"

#include <glm/glm.hpp>

namespace our {

    class lightComponent : public Component {
        public:

        glm::vec3 mLightColor = glm::vec3(1.0f , 1.0f , 1.0f); // white light color ; 

        static std::string getID() { return "Light"; }


        void deserialize(const nlohmann::json& data) override;
    };

}