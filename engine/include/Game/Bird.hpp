#ifndef BIRD_HPP
#define BIRD_HPP

#include "Objects/Interfaces/GameObject.hpp"
#include "Objects/Interfaces/Collidable.hpp"
#include "Collision/CapsuleCollider.hpp"
#include <GLFW/glfw3.h>

class Bird : public GameObject, public Collidable {
public:
    Bird();
    void update(float dt, GLFWwindow* window);  // da GameObject
    void updateColliders() override;
    void onCollision(glm::vec3 obstaclePos) override;
    bool onTerrainCollision(float terrainHeight, glm::vec3 terrainNormal);
    const CapsuleCollider& getCollider() const;
    glm::vec3 getForward() const;

    std::vector<std::shared_ptr<Collider>> getColliders() override;

    // mantém métodos específicos (opcional)
    void setModelMatrixUniform(GLuint model_uniform, const glm::mat4& view, const glm::mat4& projection) const {
        // já não será usado diretamente; a matriz é enviada por GameObject::render()
        // mantido apenas para compatibilidade, mas não chamado.
    }

    glm::vec3 getUp() const;
private:
    float rotationY, rotationX, rotationZ;
    glm::vec3 velocity;
    float speed, moveSpeed, rotationSpeed;
    float collisionCooldown;
    float terrainCollisionCooldown;

    CapsuleCollider m_collider;
};

#endif