#include "camera.hpp"
#include "../ecs/entity.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace our
{
    // Reads camera parameters from the given json object
    void CameraComponent::deserialize(const nlohmann::json &data)
    {
        if (!data.is_object())
            return;
        std::string cameraTypeStr = data.value("cameraType", "perspective");
        if (cameraTypeStr == "orthographic")
        {
            cameraType = CameraType::ORTHOGRAPHIC;
        }
        else
        {
            cameraType = CameraType::PERSPECTIVE;
        }
        // Check if the cameraType is orthographic or perspective
        near = data.value("near", 0.01f);
        far = data.value("far", 100.0f);
        fovY = data.value("fovY", 90.0f) * (glm::pi<float>() / 180);
        orthoHeight = data.value("orthoHeight", 1.0f);

        // near = data.value("near", 0.01f);
        // far = data.value("far", 100.0f);
        // fovY = data.value("fovY", 60.0f) * (glm::pi<float>() / 180);
        // orthoHeight = data.value("orthoHeight", 10.0f);
    }

    // Creates and returns the camera view matrix
    glm::mat4 CameraComponent::getViewMatrix() const
    {
        auto owner = getOwner();
        auto M = owner->getLocalToWorldMatrix();
        // TODO: (Req 8) Complete this function
        // HINT:
        //  In the camera space:
        //  - eye is the origin (0,0,0)
        //  - center is any point on the line of sight. So center can be any point (0,0,z) where z < 0. For simplicity, we let center be (0,0,-1)
        //  - up is the direction (0,1,0)
        //  but to use glm::lookAt, we need eye, center and up in the world state.
        //  Since M (see above) transforms from the camera to thw world space, you can use M to compute:
        //  - the eye position which is the point (0,0,0) but after being transformed by M
        //  - the center position which is the point (0,0,-1) but after being transformed by M
        //  - the up direction which is the vector (0,1,0) but after being transformed by M
        //  then you can use glm::lookAt
        //  These points define our camera's coordinate system
        glm::vec3 eye(0.0f, 0.0f, 0.0f);     // Camera position in camera space
        glm::vec3 center(0.0f, 0.0f, -1.0f); // Point camera looks at in camera space
        glm::vec3 up(0.0f, 1.0f, 0.0f);      // Camera's up direction in camera space

        // Transform these points to world space using M
        glm::vec3 worldEye = M * glm::vec4(eye, 1.0f);
        glm::vec3 worldCenter = M * glm::vec4(center, 1.0f);
        glm::vec3 worldUp = M * glm::vec4(up, 0.0f); // Using 0 for w since it's a direction

        // Create view matrix that transforms from world space to camera space
        return glm::lookAt(worldEye, worldCenter, worldUp);
    }

    // Creates and returns the camera projection matrix
    // "viewportSize" is used to compute the aspect ratio
    glm::mat4 CameraComponent::getProjectionMatrix(glm::ivec2 viewportSize) const
    {
        // TODO: (Req 8) Wrtie this function
        //  NOTE: The function glm::ortho can be used to create the orthographic projection matrix
        //  It takes left, right, bottom, top. Bottom is -orthoHeight/2 and Top is orthoHeight/2.
        //  Left and Right are the same but after being multiplied by the aspect ratio
        //  For the perspective camera, you can use glm::perspective
        float aspect = static_cast<float>(viewportSize.x) / viewportSize.y;
        if (cameraType == CameraType::ORTHOGRAPHIC)
        {
            float top = orthoHeight / 2.0f;
            float right = top * aspect;
            return glm::ortho(-right, right, -top, top, near, far);
        }
        else
        {
            return glm::perspective(fovY, aspect, near, far);
        }

        // return glm::mat4(1.0f);
    }
}