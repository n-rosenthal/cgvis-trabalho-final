/**
 * @file    `GameObject.hpp`
 * @brief   Interface para objetos da cena virtual
 */

#pragma once

#include <memory>
#include <iostream>

#include "Objects/Interfaces/Drawable.hpp"

class GameObject {
public:
    /**
     * @brief Construtor padrão para um GameObject
     * 
     * @param drawable  (std::unique_ptr<Drawable>)
     *      Ponteiro para uma instância de classe
     *      que implementa a interface `Drawable`
     * @param position  (glm::vec3)
     *      posição do objeto
     * @param rotation  (glm::vec3)
     *      rotação do objeto
     * @param scale     (glm::vec3)
     *      escala do objeto
     */
    GameObject(
        std::unique_ptr<Drawable> drawable,
        const glm::vec3& position = glm::vec3(0.0f),
        const glm::vec3& rotation = glm::vec3(0.0f),
        const glm::vec3& scale    = glm::vec3(1.0f))
        : 
            m_drawable(std::move(drawable)),
            m_position(position),
            m_rotation(rotation),
            m_scale(scale)
    {}

    /**
     * @brief Destruidor padrão para um GameObject
     */
    virtual ~GameObject() = default;

    // Atualiza lógica do jogo (movimento, AI, etc.)

    /**
     * @brief Atualiza lógica do jogo
     * 
     * @param deltaTime (float)
     *      tempo decorrido desde o ultimo frame
     */
    virtual void update(float deltaTime) {
        // Exemplo: mover objeto
        // m_position += m_velocity * deltaTime;
    }

    // Renderiza o objeto usando o Drawable interno

    /**
     * @brief   Renderiza o objeto
     * @details Renderiza o objeto usando o Drawable interno
     * 
     * @param ctx (const DrawContext&)
     */
    void render(const DrawContext& ctx) {
        if (m_drawable) {
            // Sincroniza transformações do GameObject com o Drawable
            // (ou passa os parâmetros para o Drawable de outra forma)
            m_drawable->setPosition(m_position);
            m_drawable->setRotation(m_rotation);
            m_drawable->setScale(m_scale);
            m_drawable->model(ctx);
        }   else {
            std::cerr << "GameObject (Tree): Drawable is null" << std::endl;
        }
    }

    // Getters / Setters para transformações
    glm::vec3 getPosition() const { return m_position; }
    void setPosition(const glm::vec3& pos) { m_position = pos; }

    // ... outros métodos (colisão, etc.)

protected:
    std::unique_ptr<Drawable> m_drawable;
    glm::vec3 m_position;
    glm::vec3 m_rotation;
    glm::vec3 m_scale;
};