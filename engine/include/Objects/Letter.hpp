/**
 * @file    `Letter.hpp`
 * @brief   Header para impl. da interface `GameObject` (objeto do jogo) para `Letter`
 */

#pragma once

#include "Objects/Interfaces/GameObject.hpp"

#include "Objects/Assets.hpp"
#include "Objects/ObjDrawable.hpp"

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
                = glm::vec3(1.0f)
    ) : GameObject(std::make_unique<ObjDrawable>(
        Assets::LETTER
    ), position, rotation, scale) {};

    //  Getters

    /**
     * @brief   Acessador à posição atual de `Letter`
     * @return  (glm::vec3)
     *          posição atual de `Letter`
     */
    glm::vec3 getPosition() const { return m_position; }

    /**
     * @brief   Retorna o tamanho da carta
     * @return  (float)
     *          tamanho da carta
     */
    float getSize() const { return m_size; }

    //  Setters
    /**
     * @brief   Define posição de `Letter`
     * @param   position (glm::vec3)
     *          posição a ser definida para `Letter`
     */
    void setPosition(const glm::vec3& position) { m_position = position; }

    void setRotation(const glm::vec3& rotation) { m_rotation = rotation; }

    /**
     * @brief   Define o tamanho de `Letter`
     * @param   size (float)
     *          tamanho a ser definido para `Letter`
     */
    void setSize(float size) { m_size = size; }


private:
    //  Tamanho da carta
    float m_size;

    //  Ponteiro para uma instância de classe
    //  que implementa a interface `Drawable`
    std::unique_ptr<ObjDrawable> m_drawable;
};