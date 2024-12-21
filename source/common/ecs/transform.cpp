#include "entity.hpp"
#include "../deserialize-utils.hpp"

#include <glm/gtx/euler_angles.hpp>
#include<glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include<glm/common.hpp>

namespace our
{

    // This function computes and returns a matrix that represents this transform
    // Remember that the order of transformations is: Scaling, Rotation then Translation
    // HINT: to convert euler angles to a rotation matrix, you can use glm::yawPitchRoll
    glm::mat4 Transform::toMat4() const
    {
        // TODO: (Req 3) Write this function
        glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scale);

        const rp3d::Quaternion& q = transform.getOrientation();
        glm::mat4 rotateMatrix = glm::toMat4(glm::quat(q.w, q.x, q.y, q.z));
        const rp3d::Vector3& pos = transform.getPosition();
        glm::vec3 position(pos.x, pos.y, pos.z);
        // Scaling, Rotation then Translation
        glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), position);
        glm::mat4 transformMatrix = translationMatrix * rotateMatrix * scaleMatrix;
        return transformMatrix;
    }

    // Deserializes the entity data and components from a json object
    void Transform::deserialize(const nlohmann::json &data)
    {
        glm::vec3 rotation(0,0,0), position(0,0,0);
        position = data.value("position", position);
        rotation = glm::radians(data.value("rotation", glm::degrees(rotation)));
        scale = data.value("scale", scale);
        transform.setPosition(rp3d::Vector3(position.x, position.y, position.z));
        transform.setOrientation(rp3d::Quaternion::fromEulerAngles(rotation.x, rotation.y, rotation.z));
    }

}