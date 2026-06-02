#include <glm/glm.hpp>
#include <glm/common.hpp>
#include <algorithm>

#include "Collision/CollisionSystem.hpp"

#include "Collision/Collider.hpp"
#include "Collision/SphereCollider.hpp"
#include "Collision/AABBCollider.hpp"
#include "Collision/CapsuleCollider.hpp"
#include "Collision/CylindricalCollider.hpp"

/**
 * @brief Verifica colisão entre dois colisores e faz dispatch
 * para o algoritmo apropriado.
 */
bool CollisionSystem::intersects(
    const Collider& a,
    const Collider& b
) {
    // =========================================
    // Sphere × Sphere
    // =========================================

    if (
        a.type() == ColliderType::Sphere &&
        b.type() == ColliderType::Sphere
    ) {
        return sphereSphere(
            static_cast<const SphereCollider&>(a),
            static_cast<const SphereCollider&>(b)
        );
    }

    // =========================================
    // Sphere × AABB
    // =========================================

    if (
        a.type() == ColliderType::Sphere &&
        b.type() == ColliderType::AABB
    ) {
        return sphereAABB(
            static_cast<const SphereCollider&>(a),
            static_cast<const AABBCollider&>(b)
        );
    }

    if (
        a.type() == ColliderType::AABB &&
        b.type() == ColliderType::Sphere
    ) {
        return sphereAABB(
            static_cast<const SphereCollider&>(b),
            static_cast<const AABBCollider&>(a)
        );
    }

    // =========================================
    // Sphere × Capsule
    // =========================================

    if (
        a.type() == ColliderType::Sphere &&
        b.type() == ColliderType::Capsule
    ) {
        return sphereCapsule(
            static_cast<const SphereCollider&>(a),
            static_cast<const CapsuleCollider&>(b)
        );
    }

    if (
        a.type() == ColliderType::Capsule &&
        b.type() == ColliderType::Sphere
    ) {
        return sphereCapsule(
            static_cast<const SphereCollider&>(b),
            static_cast<const CapsuleCollider&>(a)
        );
    }

    // =========================================
    // AABB × Capsule
    // =========================================

    if (
        a.type() == ColliderType::AABB &&
        b.type() == ColliderType::Capsule
    ) {
        return aabbCapsule(
            static_cast<const AABBCollider&>(a),
            static_cast<const CapsuleCollider&>(b)
        );
    }

    if (
        a.type() == ColliderType::Capsule &&
        b.type() == ColliderType::AABB
    ) {
        return aabbCapsule(
            static_cast<const AABBCollider&>(b),
            static_cast<const CapsuleCollider&>(a)
        );
    }

    // =========================================
    // Capsule × Capsule
    // =========================================

    if (
        a.type() == ColliderType::Capsule &&
        b.type() == ColliderType::Capsule
    ) {
        return capsuleCapsule(
            static_cast<const CapsuleCollider&>(a),
            static_cast<const CapsuleCollider&>(b)
        );
    }

    // =========================================
    // Capsule × Cylindrical
    // =========================================

    if (
        a.type() == ColliderType::Capsule &&
        b.type() == ColliderType::Cylinder
    ) {
        return capsuleCylinder(
            static_cast<const CapsuleCollider&>(a),
            static_cast<const CylindricalCollider&>(b)
        );
    }

    return false;
}

/**
 * @brief Verifica colisão entre duas esferas.
 */
bool CollisionSystem::sphereSphere(
    const SphereCollider& a,
    const SphereCollider& b
) {
    return glm::distance(a.center, b.center) <= a.radius + b.radius;
}

/**
 * @brief Verifica colisão entre esfera e AABB.
 */
bool CollisionSystem::sphereAABB(
    const SphereCollider& sphere,
    const AABBCollider& box
) {
    glm::vec3 closestPoint;

    closestPoint.x =
        glm::clamp(
            sphere.center.x,
            box.minCorner.x,
            box.maxCorner.x
        );

    closestPoint.y =
        glm::clamp(
            sphere.center.y,
            box.minCorner.y,
            box.maxCorner.y
        );

    closestPoint.z =
        glm::clamp(
            sphere.center.z,
            box.minCorner.z,
            box.maxCorner.z
        );

    glm::vec3 delta =
        sphere.center - closestPoint;

    float distanceSquared =
        glm::dot(delta, delta);

    return distanceSquared <=
           sphere.radius * sphere.radius;
}

/**
 * @brief   Verifica colisão entre esfera e capsula.
 */
bool CollisionSystem::sphereCapsule(
    const SphereCollider& sphere,
    const CapsuleCollider& capsule
) {
    return false;
}

/**
 * @brief   Verifica colisão entre AABB e capsula.
 */
bool CollisionSystem::aabbCapsule(
    const AABBCollider& aabb,
    const CapsuleCollider& capsule
) {
    return false;
}

/**
 * @brief   Verifica colisão entre capsulas.
 */
bool CollisionSystem::capsuleCapsule(
    const CapsuleCollider& a,
    const CapsuleCollider& b
) {
    return false;
}

/**
 * @brief   Verifica colisão entre um colisor capsular
 *          e um colisor cilíndrico.
 * 
 * @param a
 *          Primeiro colisor
 * @param b 
 *          Segundo colisor
 * @return true, caso haja colisão
 * @return false, caso contrário
 */
bool CollisionSystem::capsuleCylinder(const CapsuleCollider& a, const CylindricalCollider& b) {
    // =========================================
    // Eixo do cilindro
    // =========================================

    glm::vec2 cylinderXZ(
        b.center.x,
        b.center.z
    );

    glm::vec2 aXZ(
        a.p0.x,
        a.p0.z
    );

    glm::vec2 bXZ(
        a.p1.x,
        a.p1.z
    );

    // =========================================
    // Ponto mais próximo do eixo do cilindro
    // ao segmento da cápsula (projeção XZ)
    // =========================================

    glm::vec2 ab = bXZ - aXZ;

    float ab2 = glm::dot(ab, ab);

    float t = 0.0f;

    if (ab2 > 1e-6f)
    {
        t =
            glm::dot(
                cylinderXZ - aXZ,
                ab
            ) / ab2;

        t = glm::clamp(
            t,
            0.0f,
            1.0f
        );
    }

    glm::vec2 closestXZ =
        aXZ + t * ab;

    // =========================================
    // Distância horizontal
    // =========================================

    float horizontalDistance =
        glm::distance(
            closestXZ,
            cylinderXZ
        );

    if (
        horizontalDistance >
        a.radius + b.radius
    )
    {
        return false;
    }

    // =========================================
    // Verificação vertical
    // =========================================

    float capsuleMinY =
        std::min(
            a.p0.y,
            a.p1.y
        ) - a.radius;

    float capsuleMaxY =
        std::max(
            a.p0.y,
            a.p1.y
        ) + a.radius;

    float cylinderMinY =
        b.center.y -
        b.height * 0.5f;

    float cylinderMaxY =
        b.center.y +
        b.height * 0.5f;

    bool overlapY =
        capsuleMaxY >= cylinderMinY &&
        capsuleMinY <= cylinderMaxY;

    return overlapY;
}