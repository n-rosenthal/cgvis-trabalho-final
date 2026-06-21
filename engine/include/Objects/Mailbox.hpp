#pragma once

#include "Objects/StaticObject.hpp"
#include "Objects/Assets.hpp"

class Mailbox : public StaticObject {
public:
    Mailbox(
        const glm::vec3& position,
        const glm::vec3& rotation = glm::vec3(0.0f),
        const glm::vec3& scale = glm::vec3(1.0f)
    );

    void activate();
    void update(float dt);

    glm::vec3 getPosition() const { return m_position; }

private:
    bool m_activated = false;
    float m_time = 0.0f;
    glm::vec3 m_basePosition;
};