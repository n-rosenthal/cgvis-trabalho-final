#include <glm/glm.hpp>
#include <glm/common.hpp>
#include <algorithm>
#include <float.h> 

#include "Collision/CollisionSystem.hpp"

#include "Collision/Collider.hpp"
#include "Collision/SphereCollider.hpp"
#include "Collision/AABBCollider.hpp"
#include "Collision/CapsuleCollider.hpp"
#include "Collision/CylindricalCollider.hpp"
#include "Objects/Interfaces/GameObject.hpp"

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

    if (
    a.type() == ColliderType::Cylinder &&
    b.type() == ColliderType::Capsule
    ) {
        return capsuleCylinder(
            static_cast<const CapsuleCollider&>(b),
            static_cast<const CylindricalCollider&>(a)
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
// =============================================
//  sphereCapsule
// =============================================
bool CollisionSystem::sphereCapsule(
    const SphereCollider& sphere,
    const CapsuleCollider& capsule
) {
    glm::vec3 ab = capsule.p1 - capsule.p0;
    float ab2 = glm::dot(ab, ab);
    float t = 0.0f;
    if (ab2 > 1e-6f) {
        t = glm::dot(sphere.center - capsule.p0, ab) / ab2;
        t = glm::clamp(t, 0.0f, 1.0f);
    }
    glm::vec3 closestPoint = capsule.p0 + t * ab;
    float dist = glm::distance(sphere.center, closestPoint);
    return dist <= sphere.radius + capsule.radius;
}

/**
 * @brief   Verifica colisão entre AABB e capsula.
 */
bool CollisionSystem::aabbCapsule(
    const AABBCollider& aabb,
    const CapsuleCollider& capsule
) {
    // Função auxiliar: encontra o ponto mais próximo na AABB ao segmento
    auto closestPointOnSegmentToAABB = [&](const glm::vec3& p0, const glm::vec3& p1) -> glm::vec3 {
        glm::vec3 best = p0;
        float bestDist = FLT_MAX;
        const int STEPS = 20;
        for (int i = 0; i <= STEPS; ++i) {
            float t = (float)i / STEPS;
            glm::vec3 point = p0 + t * (p1 - p0);
            glm::vec3 clamped = glm::clamp(point, aabb.minCorner, aabb.maxCorner);
            float dist = glm::distance(point, clamped);
            if (dist < bestDist) {
                bestDist = dist;
                best = point;
            }
        }
        return best;
    };

    glm::vec3 closest = closestPointOnSegmentToAABB(capsule.p0, capsule.p1);
    glm::vec3 clamped = glm::clamp(closest, aabb.minCorner, aabb.maxCorner);
    float dist = glm::distance(closest, clamped);
    return dist <= capsule.radius;
}

/**
 * @brief   Verifica colisão entre capsulas.
 */
bool CollisionSystem::capsuleCapsule(
    const CapsuleCollider& a,
    const CapsuleCollider& b
) {
    glm::vec3 u = a.p1 - a.p0;
    glm::vec3 v = b.p1 - b.p0;
    glm::vec3 w = a.p0 - b.p0;
    float a_ = glm::dot(u, u);
    float b_ = glm::dot(u, v);
    float c_ = glm::dot(v, v);
    float d_ = glm::dot(u, w);
    float e_ = glm::dot(v, w);
    float D = a_ * c_ - b_ * b_;

    float sc, tc;
    if (D < 1e-6f) {
        sc = 0.0f;
        tc = (b_ > c_ ? d_ / b_ : e_ / c_);
    } else {
        sc = (b_ * e_ - c_ * d_) / D;
        tc = (a_ * e_ - b_ * d_) / D;
    }

    sc = glm::clamp(sc, 0.0f, 1.0f);
    tc = glm::clamp(tc, 0.0f, 1.0f);

    glm::vec3 closestA = a.p0 + sc * u;
    glm::vec3 closestB = b.p0 + tc * v;

    float dist = glm::distance(closestA, closestB);
    return dist <= a.radius + b.radius;
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

// =============================================
//  gameObjectsIntersect
// =============================================
bool CollisionSystem::collidablesIntersect(
    const Collidable& a,
    const Collidable& b
) {
    auto collidersA = a.getColliders();
    auto collidersB = b.getColliders();

    for (auto& ca : collidersA) {
        for (auto& cb : collidersB) {
            if (intersects(*ca, *cb)) {
                return true;
            }
        }
    }
    return false;
}