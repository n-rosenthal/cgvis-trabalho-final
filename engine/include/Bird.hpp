/** Header para implementação da Ave controlada pelo usuário */

#ifndef BIRD_HPP
#define BIRD_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

class Bird
{
public:

    // =========================================================
    // CONSTRUTOR
    // =========================================================

    Bird();

    // =========================================================
    // UPDATE
    // =========================================================

    // Atualiza física e controles da ave
    void update(float dt, GLFWwindow* window);

    // =========================================================
    // RENDER
    // =========================================================

    // Envia a matriz de modelagem do pássaro
    void setModelMatrixUniform(
        GLuint model_uniform,
        const glm::mat4& view,
        const glm::mat4& projection
    ) const;

    // =========================================================
    // COLISÃO
    // =========================================================

    // Trata colisão contra obstáculos
    void onCollision(glm::vec3 obstaclePos);

    // =========================================================
    // GETTERS
    // =========================================================

    // posição atual
    glm::vec3 getPosition() const
    {
        return position;
    }

    // direção para frente
    glm::vec3 getForward() const;

    // velocidade escalar
    float getSpeed() const
    {
        return speed;
    }

    // velocidade vetorial
    glm::vec3 getVelocity() const
    {
        return velocity;
    }

    // raio de colisão
    float getCollisionRadius() const
    {
        return collisionRadius;
    }

    // yaw
    float getRotationY() const
    {
        return rotationY;
    }

    // verifica crash
    bool hasCrashed() const
    {
        return crashed;
    }

    // =========================================================
    // COLISÃO
    // =========================================================

    float collisionRadius;

    // cooldown entre colisões
    float collisionCooldown;
    float terrainCollisionCooldown;

    // estado de crash
    bool crashed;

    // colisão contra terreno
    bool onTerrainCollision(float terrainHeight, glm::vec3 terrainNormal);
private:

    // =========================================================
    // TRANSFORMAÇÃO
    // =========================================================

    glm::vec3 position;

    // rotações
    float rotationY; // yaw
    float rotationX; // pitch
    float rotationZ; // roll

    // =========================================================
    // FÍSICA
    // =========================================================

    glm::vec3 velocity;
    glm::vec3 acceleration;

    float speed;

    // =========================================================
    // CONTROLE
    // =========================================================

    float moveSpeed;
    float rotationSpeed;


};

#endif