/**
 * @file    `Letter.cpp`
 * @brief   Implementação de `Letter` enquanto objeto do jogo.
 */

#include "Objects/Letter.hpp"
#include "Objects/ObjDrawable.hpp"
#include "glad/glad.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>

#include "Objects/Letter.hpp"

Letter::Letter(
    const glm::vec3& position,
    const glm::vec3& rotation,
    const glm::vec3& scale
)
:
GameObject(
    std::make_unique<ObjDrawable>(Assets::LETTER),
    position,
    rotation,
    scale
)
{
    m_collider =
        std::make_shared<SphereCollider>(
            position,
            1.0f
        );

    updateColliders();
}

std::vector<std::shared_ptr<Collider>>
Letter::getColliders() const
{
    return { m_collider };
}

void Letter::updateColliders()
{
    m_collider->center = m_position;
    m_collider->radius = m_size;
}

void Letter::onCollision(glm::vec3)
{
    // Nada por enquanto
}

void Letter::setPosition(const glm::vec3& pos)
{
    m_position = pos;
    updateColliders();
}

void Letter::setRotation(const glm::vec3& rot)
{
    m_rotation = rot;
}

void Letter::setSize(float size)
{
    m_size = size;
    updateColliders();
}