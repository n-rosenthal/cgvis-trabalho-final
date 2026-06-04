#include "Objects/Tree.hpp"
#include "Objects/Drawables/TreeDrawable.hpp"

Tree::Tree(const glm::vec3& position,
           const glm::vec3& rotation,
           const glm::vec3& scale,
           int type)
    : GameObject(std::make_unique<TreeDrawable>(type), position, rotation, scale)
    , m_type(type)
    , m_trunkHeight(4.0f * scale.y)
    , m_trunkRadius(0.5f * scale.x)
    , m_canopyRadius(2.0f * scale.x)
    , m_trunkCollider(position, m_trunkRadius, m_trunkHeight)
    , m_canopyCollider(position + glm::vec3(0.0f, m_trunkHeight, 0.0f), m_canopyRadius)
{
    // Gera os buffers do drawable (mesmo que vazios, por segurança)
    if (m_drawable) m_drawable->generate();
}

std::vector<std::shared_ptr<Collider>> Tree::getColliders() {
    std::vector<std::shared_ptr<Collider>> colliders;
    colliders.push_back(std::make_shared<CylindricalCollider>(m_trunkCollider));
    colliders.push_back(std::make_shared<SphereCollider>(m_canopyCollider));
    return colliders;
}

void Tree::onCollision(glm::vec3 objectPosition) {
    // Reação à colisão (som, efeito visual...)
}

void Tree::updateColliders() {
    m_trunkCollider.center = m_position;
    m_trunkCollider.radius = m_trunkRadius;
    m_trunkCollider.height = m_trunkHeight;

    m_canopyCollider.center = m_position + glm::vec3(0.0f, m_trunkHeight, 0.0f);
    m_canopyCollider.radius = m_canopyRadius;
}

void Tree::update(float dt) {
    GameObject::update(dt);   // se houver lógica no futuro
    updateColliders();        // mantém colisores sincronizados com a posição
}