#pragma once

#include <glm/glm.hpp>

#include "Collider.hpp"

/**
 * @brief Colisor esférico.
 */
class SphereCollider : public Collider
{
public:

    glm::vec3 center;
    float radius;

public:

    /**
     * @brief Construtor padrão.
     */
    SphereCollider()
        : center(0.0f),
          radius(1.0f)
    {
    }

    /**
     * @brief Construtor parametrizado.
     */
    SphereCollider(
        const glm::vec3& c,
        float r
    )
        : center(c),
          radius(r)
    {
    }

    /**
     * @brief Tipo do colisor.
     */
    ColliderType type() const override
    {
        return ColliderType::Sphere;
    }
};