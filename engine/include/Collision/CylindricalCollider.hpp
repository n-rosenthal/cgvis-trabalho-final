#pragma once

#include <glm/glm.hpp>

#include "Collision/Collider.hpp"

/**
 * @brief Colisor cilíndrico vertical.
 */
class CylindricalCollider : public Collider
{
public:

    glm::vec3 center;
    float radius;
    float height;

public:

    /**
     * @brief Construtor padrão.
     */
    CylindricalCollider()
        : center(0.0f),
          radius(1.0f),
          height(1.0f)
    {
    }

    /**
     * @brief Construtor parametrizado.
     */
    CylindricalCollider(
        const glm::vec3& c,
        float r,
        float h
    )
        : center(c),
          radius(r),
          height(h)
    {
    }

    ColliderType type() const override
    {
        return ColliderType::Cylinder;
    }
};