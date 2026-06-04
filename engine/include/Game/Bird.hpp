#ifndef BIRD_HPP
#define BIRD_HPP

#include "Objects/Interfaces/GameObject.hpp"
#include "Objects/Interfaces/Collidable.hpp"
#include "Collision/CapsuleCollider.hpp"
#include <GLFW/glfw3.h>

class Bird : public GameObject, public Collidable {
public:
    Bird();

    void update(float dt, GLFWwindow* window);
    void updateColliders() override;
    void onCollision(glm::vec3 obstaclePos) override;
    bool onTerrainCollision(float terrainHeight, glm::vec3 terrainNormal);

    glm::vec3 getForward() const;
    glm::vec3 getUp() const;
    const CapsuleCollider& getCollider() const;
    std::vector<std::shared_ptr<Collider>> getColliders() override;

private:
    // Orientação (em radianos)
    float m_yaw   = 0.0f;   // rotação horizontal (Y)
    float m_pitch = 0.0f;   // nariz cima/baixo  (X)
    float m_roll  = 0.0f;   // inclinação lateral (Z)

    // Movimento
    glm::vec3 m_velocity  = glm::vec3(0.0f);
    float     m_speed     = 18.0f;   // velocidade escalar atual

    // Cooldowns de colisão
    float m_hitCooldown     = 0.0f;
    float m_terrainCooldown = 0.0f;

    // Estado do flap (detecta borda de subida)
    bool m_flapHeld = false;

    CapsuleCollider m_collider;

    // Helpers
    glm::mat4 rotationMatrix() const;
    void      clampPosition();
};

#endif