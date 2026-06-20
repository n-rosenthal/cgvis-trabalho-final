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
    std::vector<std::shared_ptr<Collider>> colliders
)
    : GameObject(
            std::make_unique<ObjDrawable>(model),
            position,
            rotation,
            scale
        ),
        m_colliders(std::move(colliders))
{
}

// Construtor com um único collider (delega para o anterior)
StaticObject::StaticObject(
    const ModelDefinition& model,
    const glm::vec3& position,
    const glm::vec3& rotation,
    const glm::vec3& scale,
    std::shared_ptr<Collider> collider
)
    : StaticObject(model, position, rotation, scale, std::vector{std::move(collider)})
{
}

std::vector<std::shared_ptr<Collider>> StaticObject::getColliders() const {
    return m_colliders;
}

void StaticObject::updateColliders() {
    // Se o objeto puder se mover (embora estático), atualize a posição dos colliders
    // Exemplo: para uma esfera, atualizar o centro
    for (auto& coll : m_colliders) {
        if (coll->type() == ColliderType::Sphere) {
            auto sphere = std::dynamic_pointer_cast<SphereCollider>(coll);
            if (sphere) {
                sphere->center = m_position;
            }
        }
        //...
    }
}

void StaticObject::onCollision(glm::vec3 objectPosition) {
    // (som, partículas)
    (void)objectPosition;
}