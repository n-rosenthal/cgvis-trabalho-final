#include "Objects/StaticObject.hpp"

//  Colisores
#include "Collision/SphereCollider.hpp"
#include "Collision/AABBCollider.hpp"
#include "Collision/CapsuleCollider.hpp"
#include "Collision/CylindricalCollider.hpp"

// Construtor com múltiplos colisores
StaticObject::StaticObject(
    const ModelDefinition& model,
    const glm::vec3& position,
    const glm::vec3& rotation,
    const glm::vec3& scale,
    StaticObjectType type,                               // <-- novo
    std::vector<std::shared_ptr<Collider>> colliders
)
    : GameObject(
          std::make_unique<ObjDrawable>(model),
          position,
          rotation,
          scale
      ),
      m_colliders(std::move(colliders)),
      m_type(type)                                        // <-- inicializa
{
}

// Construtor com um único collider
StaticObject::StaticObject(
    const ModelDefinition& model,
    const glm::vec3& position,
    const glm::vec3& rotation,
    const glm::vec3& scale,
    StaticObjectType type,                               // <-- novo
    std::shared_ptr<Collider> collider
)
    : StaticObject(model, position, rotation, scale, type, std::vector{std::move(collider)})
{
}

std::vector<std::shared_ptr<Collider>> StaticObject::getColliders() const {
    return m_colliders;
}

void StaticObject::updateColliders() {
    for (auto& coll : m_colliders) {
        switch (coll->type()) {
            case ColliderType::Sphere: {
                auto sphere = std::dynamic_pointer_cast<SphereCollider>(coll);
                if (sphere) {
                    // O centro local é relativo à posição do objeto
                    sphere->center = m_position + sphere->center; // se center for local
                }
                break;
            }
            case ColliderType::AABB: {
                auto aabb = std::dynamic_pointer_cast<AABBCollider>(coll);
                if (aabb) {
                    aabb->minCorner += m_position;
                    aabb->maxCorner += m_position;
                }
                break;
            }
            case ColliderType::Capsule: {
                auto capsule = std::dynamic_pointer_cast<CapsuleCollider>(coll);
                if (capsule) {
                    capsule->p0 += m_position;
                    capsule->p1 += m_position;
                }
                break;
            }
            case ColliderType::Cylinder: {
                auto cylinder = std::dynamic_pointer_cast<CylindricalCollider>(coll);
                if (cylinder) {
                    cylinder->center += m_position;
                }
                break;
            }
            default: break;
        }
    }
}

void StaticObject::onCollision(glm::vec3 objectPosition) {
    // (som, partículas)
    (void)objectPosition;
}