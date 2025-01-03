#pragma once
#include "../ecs/component.hpp"
#include "../deserialize-utils.hpp"

namespace r3d = reactphysics3d;
namespace our {
    class RigidBodyComponent : public Component {
        r3d::RigidBody* body = nullptr;
        r3d::Collider* collider = nullptr;
        void deserialize_collider(const nlohmann::json& data);
    public:
        bool isPlayer = false;
        static std::string getID() { return "RigidBody"; }
        void deserialize(const nlohmann::json &data) override;
        r3d::RigidBody* getBody() { return body; }
        ~RigidBodyComponent();
    };
}