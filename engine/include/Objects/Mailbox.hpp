#pragma once

#include <glm/glm.hpp>
#include "Objects/StaticObject.hpp"


class Mailbox : public StaticObject {
public:
    Mailbox(
        const glm::vec3& position,
        const glm::vec3& rotation,
        const glm::vec3& scale
    );

    void update(float dt) override;

    void activate();

    bool activated() const;

private:
    bool  m_activated = false;
    float m_time = 0.0f;

    glm::vec3 m_basePosition;
};