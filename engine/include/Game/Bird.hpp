/** Header para implementação da Ave controlada pelo usuário */

#ifndef BIRD_HPP
#define BIRD_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

//  Pássaro é descrito através do modelo capsular para o sistema de colisões
#include "Collision/CapsuleCollider.hpp"

class Bird {
public:
    /**
     * @brief Construtor padrão para o pássaro
     */
    Bird();

    /**
     * @brief   Atualização do modelo do pássaro ao longo do jogo
     * 
     * @param dt (float)
     *          delta time
     * @param window (GLFWwindow*)
     *          ponteiro para janela do jogo (OpenGL)
     */
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
    //  Retorna o colisor do pássaro
    const CapsuleCollider& getCollider() const;
    CapsuleCollider& getCollider();

    //  Atualiza o colisor do pássaro
    void updateCollider();

    //  Consequências de uma colisão, para o pássaro
    void onCollision(glm::vec3 obstaclePos);

    //  Consequências de uma colisão contra o terreno, para o pássaro
    bool onTerrainCollision(
        float terrainHeight,
        glm::vec3 terrainNormal
    );


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

    // yaw
    float getRotationY() const
    {
        return rotationY;
    }
    

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

    // =========================================
    // COLISÃO
    // =========================================

    CapsuleCollider m_collider;

    float collisionCooldown;
    float terrainCollisionCooldown;
};

#endif