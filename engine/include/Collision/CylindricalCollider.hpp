#pragma once

#include <glm/glm.hpp>

#include "Collision/Collider.hpp"

/**
 * @brief Colisor cilíndrico vertical.
 */
class CylindricalCollider : public Collider
{
public:
    glm::vec3 localCenter; // relativo ao objeto
    glm::vec3 center;      // posição no mundo

    float radius;
    float height;
public:
    CylindricalCollider(
        const glm::vec3& c,
        float r,
        float h
    )
    :
        localCenter(c),
        center(c),
        radius(r),
        height(h)
    {
    }

    

    ColliderType type() const override
    {
        return ColliderType::Cylinder;
    }
};