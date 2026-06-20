#pragma once

/**
 * @file        `Bird.hpp`
 * @brief       Header para a classe `Bird`, personagem controlável pelo jogador
 */

#ifndef BIRD_HPP
#define BIRD_HPP

#include <memory>
#include <vector>

#include "Objects/Assets.hpp"

#include <GLFW/glfw3.h>

#include "Objects/Interfaces/GameObject.hpp"
#include "Objects/Interfaces/Collidable.hpp"
#include "Objects/ObjDrawable.hpp"
#include "Collision/CapsuleCollider.hpp"
#include "Objects/Letter.hpp"

class Bird : public GameObject, public Collidable {
public:
    /**
     * @brief Construtor padrão para `Bird`
     */
    Bird();

    /**
     * @brief   Atualizador dinâmico para `Bird`
     * 
     * @param   dt  (float)
     *          passagem do tempo
     * @param   window (GLFWwindow*)
     *          ponteiro para a janela
     * @param   terrainHeight (float)
     *          altura do terreno
     * @param   terrainNormal (glm::vec3)
     *          vetor normal ao terreno
     */
    void update(float dt,
                GLFWwindow* window,
                float terrainHeight,
                glm::vec3 terrainNormal);

    /**
     * @brief   Atualizador para o colisor de `Bird`
     */
    void updateColliders() override;

    /**
     * @brief   Implementação das consequências de uma colisão
     *          contra um obstáculo
     * 
     * @param   obstaclePos (glm::vec3)
     *          posição do obstáculo
     */
    void onCollision(glm::vec3 obstaclePos) override;

    /**
     * @brief   Implementação das consequências de uma colisão
     *          contra o terreno
     * 
     * @param   terrainHeight (float)
     *          altura do terreno
     * @param   terrainNormal (glm::vec3)
     *          vetor normal ao terreno na posição de colisão
     * 
     * @return  true, se o pássaro colidiu com o chão
     *          false, caso contrário
     */
    bool onTerrainCollision(float terrainHeight, glm::vec3 terrainNormal);

    /**
     * @brief   Acessador para obter o vetor "direção para frente"
     *          de `Bird`
     * 
     * @return  glm::vec3
     *          direção para frente, aonde aponta o bico do pássaro
     */
    glm::vec3 getForward() const;

    /**
     * @brief   Acessador para obter o vetor "direção para cima"
     *          de `Bird`
     * 
     * @return  glm::vec3
     *          direção para cima, aonde aponta o topo da
     *          cabeça do pássaro
     */
    glm::vec3 getUp() const;
    
    /**
     * @brief   Atualiza o modelo de `Bird` conforme o estado atual
     * @details Se está sobre o chão, voando -> pousado
     *          Senão, pousado -> voando
     */
    void updateDrawable();

    /**
     * @brief   Acessador para obter a colisor de `Bird`
     * 
     * @return  const CapsuleCollider&
     *          colisor de `Bird`
     */
    const CapsuleCollider& getCollider() const;

    /**
     * @brief   Acessador para obter os colisores de `Bird`
     * 
     * @return std::vector<std::shared_ptr<Collider>> 
     */
    std::vector<std::shared_ptr<Collider>> getColliders() override;

    /**
     * @brief   Acessador para obter a flag que determina
     *          se `Bird` está pousado (standing, parado)
     *          ou voando
     * 
     * @return  true, se `Bird` estiver pousado
     * @return  false, se `Bird` estiver voando
     */
    bool getStanding() const {return standing; }

    /**
     * @brief   Acessador para obter o tamanho de `Bird`
     * 
     * @return  glm::vec3
     *          tamanho de `Bird`
     */
    glm::vec3 getSize() const{return size; }

    /**
     * @brief   Acessador à altura base de `Bird`
     * @details Utilizado quando `Bird` estiver pousado
     *          para obter sua altura ao caminhar, sem
     *          se modificar a câmera junto ao caminhado
     */
    float getBaseHeight() const {return m_baseY; }

    /**
     * @brief   Direção para frente, ignorando o roll de
     *          balanço lateral (bob) usado na animação
     *          de caminhar. Usado pela câmera: como o roll
     *          entra na composição da matriz de rotação
     *          (Rz * Rx * Ry), ele também desvia o vetor
     *          forward, não só o up — por isso a câmera
     *          precisa desta versão "limpa".
     * 
     * @return  glm::vec3
     */
    glm::vec3 getForwardNoBob() const;

    /**
     * @brief   Direção para cima, ignorando o roll de
     *          balanço lateral (bob) usado na animação
     *          de caminhar. Usado pela câmera para evitar
     *          que ela incline junto com a animação.
     * 
     * @return  glm::vec3
     */
    glm::vec3 getUpNoBob() const;

    /**
     * @brief   Posição sem bob de `Bird` ao caminhar
     * 
     * @return glm::vec3 
     */
    glm::vec3 getPositionNoBob() const { return glm::vec3(m_position.x, m_baseY, m_position.z); }

    /**
     * @brief   Configura o tamanho de `Bird` conforme
     *          o parâmetro tri-dimensional `s`
     * 
     * @param   s (glm::vec3)
     *          novo tamanho
     */
    void setSize(const glm::vec3& s) {size = s;}

    /**
     * @brief   Configura a rotação de `Bird` conforme
     *          o parâmetro tri-dimensional `r`
     * 
     * @param   r (glm::vec3)
     *          nova rotação
     */
    void setRotation(const glm::vec3& r) {m_rotation = r;}


    //  Constantes
    //  Tamanho padrão do modelo de `Bird` voando
    static constexpr float DEFAULT_SIZE          = 1.0f;

    //  Fator de escala do modelo de `Bird` parado
    static constexpr float STANDING_SCALE_FACTOR = 0.2f;

private:
    /**
     * @brief   Obtém a matriz de rotação de `Bird`
     * 
     * @return  glm::mat4 
     *              matriz de rotação
     */
    glm::mat4 rotationMatrix() const;

    /**
     * @brief   Obtém a matriz de rotação de `Bird`, sem o
     *          roll de bob do andar. Ver rotationMatrixNoBob()
     *          em Bird.cpp para detalhes.
     * 
     * @return  glm::mat4 
     */
    glm::mat4 rotationMatrixNoBob() const;

    /**
     * @brief   Limita a posição de `Bird` ao chão
     */
    void clampPosition();

    //  yaw: rotação em torno do eixo y
    float m_yaw   = 0.0f;

    //  pitch: rotação em torno do eixo x
    float m_pitch = 0.0f;
    
    //  roll: rotação em torno do eixo z
    float m_roll  = 0.0f;

    //  Entrada de rotação suavizada
    float m_smoothYawInput   = 0.0f;
    float m_smoothPitchInput = 0.0f;

    //  velocidade vetorial para `Bird`
    glm::vec3 m_velocity =
        glm::vec3(0.0f);

    //  magnitude da velocidade para `Bird`
    float m_speed = 0.0f;

    //  Cooldowns para colisão contra obstáculos
    //  e contra o terreno
    float m_hitCooldown     = 0.0f;
    float m_terrainCooldown = 0.0f;
    
    //  Verdadeiro se <SPACE> foi apertado recentemente
    bool m_flapHeld = false;
    
    //  Colisor para `Bird`
    CapsuleCollider m_collider;

    //  Definição de modelo para `Bird`
    //      arquivo OBJ, meshes, texturas etc.
    const ModelDefinition*
        m_currentModel;

    //  Tamanho de `Bird`
    glm::vec3 size = glm::vec3(DEFAULT_SIZE);

    //  Verdadeiro se `Bird` estiver pousado
    bool standing = true;

    //  Animação para caminhar
    float m_walkTime = 0.0f;
    float m_baseY    = 0.0f;
};

#endif