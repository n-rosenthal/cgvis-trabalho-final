#pragma once

#include <glm/glm.hpp>

#include "Collider.hpp"


/**
 * @brief   Colisor retangular (AABB, bounding-box)
 */
class AABBCollider : public Collider {
public:
    //  cantos, corners
    glm::vec3 minCorner;    //  "menor canto"
    glm::vec3 maxCorner;    //  "maior canto"

    /**
     * @brief   Construtor padrão para um colisor retangular (AABB, bounding-box)
     * 
     * @param min (glm::vec3)
     *          menor canto
     * @param max (glm::vec3)
     *          maior canto
     */
    AABBCollider(glm::vec3 min, glm::vec3 max) : minCorner(min), maxCorner(max) {}

    /**
     * @brief   Retorna o tipo de colisão
     * 
     * @return  ColliderType::AABB
     */
    ColliderType type() const override {
        return ColliderType::AABB;
    }
};