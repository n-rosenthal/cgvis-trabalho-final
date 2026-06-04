#ifndef TREE_HPP
#define TREE_HPP

#include "Interfaces/GameObject.hpp"
#include "Interfaces/Collidable.hpp"
#include "Collision/CylindricalCollider.hpp"
#include "Collision/SphereCollider.hpp"

class Tree : public GameObject, public Collidable {
public:
    Tree(const glm::vec3& position,
         const glm::vec3& rotation,
         const glm::vec3& scale,
         int type);

    // Collidable interface
    std::vector<std::shared_ptr<Collider>> getColliders() override;
    void onCollision(glm::vec3 objectPosition) override;
    void updateColliders() override;

    // Opcional: lógica de jogo
    void update(float dt) override;

private:
    int m_type;
    float m_trunkHeight;
    float m_trunkRadius;
    float m_canopyRadius;
    CylindricalCollider m_trunkCollider;
    SphereCollider      m_canopyCollider;
};

#endif