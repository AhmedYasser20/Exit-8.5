#pragma once

#include <glm/glm.hpp>
#include <json/json.hpp>
#include <glm/gtc/quaternion.hpp>
#include <reactphysics3d/reactphysics3d.h>
namespace rp3d = reactphysics3d;

namespace our {

    // A transform defines the translation, rotation & scale of an object relative to its parent
    struct Transform {
        rp3d::Transform transform;

    public:
        //glm::vec3 position = glm::vec3(0, 0, 0); // The position is defined as a vec3. (0,0,0) means no translation
        //glm::vec3 rotation = glm::vec3(0, 0, 0); // The rotation is defined using euler angles (y: yaw, x: pitch, z: roll). (0,0,0) means no rotation
        glm::vec3 scale = glm::vec3(1, 1, 1); // The scale is defined as a vec3. (1,1,1) means no scaling.

        // This function computes and returns a matrix that represents this transform
        glm::mat4 toMat4() const;
         // Deserializes the entity data and components from a json object
        void deserialize(const nlohmann::json&);

        const rp3d::Transform& getTransform() const {
            return transform;
        }
        const rp3d::Vector3& getPosition() const {
            return transform.getPosition();
        }
        const rp3d::Quaternion& getRotation() const {
            return transform.getOrientation();
        }
        void setPosition(const glm::vec3 position) {
            transform.setPosition(rp3d::Vector3(position.x, position.y, position.z));
        }
        void setPosition(const rp3d::Vector3 position) {
            transform.setPosition(position);
        }
        void setRotation(const glm::quat rotation) {
            transform.setOrientation(rp3d::Quaternion(rotation.x, rotation.y, rotation.z, rotation.w));
        }
        void setRotation(const rp3d::Quaternion rotation) {
            transform.setOrientation(rotation);
        }
        void setTransform(const rp3d::Transform transform) {
            this->transform = transform;
        }
    };
}