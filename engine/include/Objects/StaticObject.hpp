#pragma once

#include "Objects/Interfaces/GameObject.hpp"
#include "Objects/Interfaces/Collidable.hpp"
#include "Objects/ObjDrawable.hpp"
#include "Objects/Assets.hpp"

#include "Collision/Collider.hpp"
#include <memory>
#include <vector>

class StaticObject : public GameObject, public Collidable {
public:
    StaticObject(
        const ModelDefinition& model,
        const glm::vec3& position,
        const glm::vec3& rotation,
        const glm::vec3& scale,
        std::vector<std::shared_ptr<Collider>> colliders = {}
    );

    StaticObject(
        const ModelDefinition& model,
        const glm::vec3& position,
        const glm::vec3& rotation,
        const glm::vec3& scale,
        std::shared_ptr<Collider> collider
    );

    // ---- Interface Collidable ----
    std::vector<std::shared_ptr<Collider>> getColliders() const override;
    void updateColliders() override;
    void onCollision(glm::vec3 objectPosition) override;

private:
    std::vector<std::shared_ptr<Collider>> m_colliders;
};