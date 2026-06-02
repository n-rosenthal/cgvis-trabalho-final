#pragma once

#include <glm/glm.hpp>

#include "Collider.hpp"

/**
 * @brief   Colisor capsular
 */
class CapsuleCollider : public Collider {
public:
    //  pontos extremos
    glm::vec3 p0;
    glm::vec3 p1;

    //  raio da cápsula
    float radius;

    /**
     * @brief   Construtor padrão para colisor capsular
     * @param p0 (glm::vec3)
     *          primeiro ponto
     * 
     * @param p1 (glm::vec3)
     *          segundo ponto
     * 
     * @param r (float)
     *          raio da cápsula
     */
    CapsuleCollider(glm::vec3 p0, glm::vec3 p1, float r) : 
        p0(p0),
        p1(p1),
        radius(r) {}

    /**
     * @brief   Acessador para tipo do colisor
     * 
     * @return  ColliderType::Capsule
     */
    ColliderType type() const override
    {
        return ColliderType::Capsule;
    }
};