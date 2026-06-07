/**
 * @file    `Letter.hpp`
 * @brief   Header para impl. da interface `GameObject` (objeto do jogo) para `Letter`
 */

#pragma once

#include "Objects/Interfaces/GameObject.hpp"
#include "Objects/Drawables/LetterDrawable.hpp"

#include <memory>
#include <iostream>

class Letter : public GameObject {
public:
    /**
     * @brief Construtor padrão para `Letter`
     * 
     * @param   position (glm::vec3)
     *          posição inicial 
     * @param   rotation (glm::vec3)
     *          rotação inicial
     * @param   scale (glm::vec3)
     *          escala inicial  
     */
    Letter(
        const glm::vec3& position 
                = glm::vec3(5.0f, -0.9f, 5.0f),
        const glm::vec3& rotation
                = glm::vec3(0.0f),
        const glm::vec3& scale
                = glm::vec3(1.0f));

    /**
     * @brief   Atualizador dinâmico para `Letter`
     * @param   dt  (float)
     *          passagem do tempo
     * 
     * @param   bird_position (glm::vec3)
     *          posição atual do pássaro
     * 
     * @param   bird_rotation (glm::vec3)
     *          rotação atual do pássaro
     * 
     * @param   bird_captured (bool)
     *          verdadeiro, se o pássaro capturou a carta;
     *          falso, caso contrário
     */
    void update(float dt,
                const glm::vec3& bird_position,
                const glm::vec3& bird_rotation,
                bool bird_captured);

    //  Getters

    /**
     * @brief   Acessador à posição atual de `Letter`
     * @return  (glm::vec3)
     *          posição atual de `Letter`
     */
    glm::vec3 getPosition() const { return m_position; }

    /**
     * @brief   Acessador à flag indicativa de captura de `Letter`
     * @return  (bool)
     *          verdadeiro, se a carta foi capturada pelo pássaro;
     *          falso, caso contrário
     */
    bool isCaptured() const { return m_captured; }


    /**
     * @brief   Retorna o tamanho da carta
     * @return  (float)
     *          tamanho da carta
     */
    float getSize() const { return m_size; }

    //  Setters

    /**
     * @brief   Define flag indicativa de captura de `Letter`
     * @param   captured (bool)
     *          verdadeiro, se a carta foi capturada pelo pássaro;
     *          falso, caso contrário
     */
    void setCaptured(bool captured) { m_captured = captured; }

    /**
     * @brief   Define posição de `Letter`
     * @param   position (glm::vec3)
     *          posição a ser definida para `Letter`
     */
    void setPosition(const glm::vec3& position) { m_position = position; }

    /**
     * @brief   Define o tamanho de `Letter`
     * @param   size (float)
     *          tamanho a ser definido para `Letter`
     */
    void setSize(float size) { m_size = size; }

private:
    //  Velocidade vetorial
    glm::vec3 m_velocity;

    //  Tamanho da carta
    float m_size;

    //  Flag indicativa de captura da carta
    bool m_captured;

    //  Ponteiro para uma instância de classe
    //  que implementa a interface `Drawable`
    std::unique_ptr<Drawable> m_drawable;

    //  Física da carta
    const float gravity = -9.81f;
    const float ground_y = -1.0f;   // altura do terreno
    const float claw_distance = 0.5f; // distância máxima para capturar a carta
};