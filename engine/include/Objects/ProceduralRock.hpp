/**
 * @file    ProceduralRock.hpp
 * @brief   Rocha procedural como GameObject.
 *          A geometria vive no RockDrawable; esta classe
 *          cuida apenas de posição, escala e colisão.
 */
#pragma once

#include "Objects/Interfaces/GameObject.hpp"
#include "Objects/Interfaces/Collidable.hpp"

#include "Collision/SphereCollider.hpp"

class ProceduralRock : public GameObject, public Collidable {
public:
    ProceduralRock(glm::vec3 position, float scale, int subdivisions = 2);

    // Collidable
    std::vector<std::shared_ptr<Collider>> getColliders() const override;
    void onCollision(glm::vec3 objectPosition) override;
    void updateColliders() override;

    float getCollisionRadius() const { return m_collisionRadius; }

private:
    float m_collisionRadius;
};