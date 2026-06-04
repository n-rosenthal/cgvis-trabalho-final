#pragma once

/**
 * @file    `Camera.hpp`
 * @brief   Header para implementação de câmeras ao jogo.
 */

#include <glm/glm.hpp>

/**
 * @brief   Uma câmera é definida por
 * @param   m_position (glm::vec3)
 *          posição da câmera na cena virtual
 * @param   m_target (glm::vec3)
 *          posição alvo da câmera na cena virtual
 * @param   m_distance (float)
 *          distância da câmera ao alvo
 * @param   m_height (float)
 *          altura da câmera em relação ao alvo
 * @param   m_smoothness (float)
 *          suavidade da transição da câmera
 * 
 * e implementa os seguintes métodos:
 * @fn      void update(const glm::vec3& targetPos, const glm::vec3& targetForward, const glm::vec3& targetUp, float dt)
 *          atualização dinâmica da câmera conforme passagem do tempo
 * @fn      glm::mat4 getViewMatrix() const
 *          acessador à matriz view da câmera
 */
class Camera {
public:
    /**
     * @brief Construtor padrão para câmera
     */
    Camera();

    /**
     * @brief Atualizador dinâmico da câmera
     * 
     * @param targetPos     (glm::vec3)
     *          nova posição do alvo
     * @param targetForward (glm::vec3)
     *          novo forward (vetor direção) do alvo
     * @param targetUp      (glm::vec3)
     *          novo up (vetor vertical) do alvo
     * @param dt            (float)
     *          passagem do tempo
     */
    void update(
        const glm::vec3& targetPos,
        const glm::vec3& targetForward,
        const glm::vec3& targetUp,
        float dt
    );

    /**
     * @brief Acessador à matriz view da câmera
     * 
     * @return glm::mat4
     *          matriz view da câmera
     */
    glm::mat4 getViewMatrix() const;


private:
    //  Matriz de posição e de alvo
    glm::vec3 m_position;
    glm::vec3 m_target;

    //  Distância e altura
    float m_distance = 7.0f;
    float m_height   = 2.5f;

    //  Suavidade de transição
    float m_smoothness = 6.0f;
};