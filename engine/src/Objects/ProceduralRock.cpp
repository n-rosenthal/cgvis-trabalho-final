/**
 * @file    ProceduralRock.cpp
 */
#include "Objects/ProceduralRock.hpp"
#include "Objects/Drawables/RockDrawable.hpp"

ProceduralRock::ProceduralRock(glm::vec3 position, float scale, int subdivisions)
    : GameObject(std::make_unique<RockDrawable>(subdivisions),
                 position,
                 glm::vec3(0.0f),
                 glm::vec3(scale))
    , m_collisionRadius(scale * 0.95f)
{
    if (m_drawable) m_drawable->generate();
}

std::vector<std::shared_ptr<Collider>> ProceduralRock::getColliders() const {
    return {std::make_shared<SphereCollider>(
            m_position,
            m_collisionRadius
        )
    };
}

void ProceduralRock::onCollision(glm::vec3 objectPosition) {}


void ProceduralRock::updateColliders() {}