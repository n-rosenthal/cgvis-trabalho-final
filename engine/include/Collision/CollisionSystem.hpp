#pragma once

#include <glm/glm.hpp>

#include "Collider.hpp"
#include "SphereCollider.hpp"
#include "AABBCollider.hpp"
#include "CapsuleCollider.hpp"
#include "CylindricalCollider.hpp"

/**
 * @brief   Sistema de colisão
 */
class CollisionSystem {
public:
    /**
     * @brief   Verifica a colisão entre dois colisores
     * 
     * @param a
     *          Primeiro colisor
     * @param b 
     *          Segundo colisor
     * @return true, caso haja colisão
     * @return false, caso contrário
     */
    static bool intersects(const Collider& a, const Collider& b);

    /**
     * @brief   Verifica a colisão entre dois colisores esféricos
     */
    static bool sphereSphere(const SphereCollider& a, const SphereCollider& b);

    /**
     * @brief   Verifica a colisão entre um colisor esférico e um colisor retangular (AABB, bounding-box)
     */
    static bool sphereAABB(const SphereCollider& a, const AABBCollider& b);

    /**
     * @brief   Verifica a colisão entre um colisor esférico e um colisor capsular
     */
    static bool sphereCapsule(const SphereCollider& a, const CapsuleCollider& b);

    /**
     * @brief   Verifica a colisão entre um colisor retangular (AABB, bounding-box) e um colisor capsular
     */
    static bool aabbCapsule(const AABBCollider& a, const CapsuleCollider& b);

    /**
     * @brief   Verifica a colisão entre dois colisores capsulares
     */
    static bool capsuleCapsule(const CapsuleCollider& a, const CapsuleCollider& b);

    /**
     * @brief   Verifica a colisão entre um colisor capsular e um colisor cilíndrico
    */
    static bool capsuleCylinder(const CapsuleCollider& a, const CylindricalCollider& b);

};