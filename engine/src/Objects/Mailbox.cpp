#include "Objects/Mailbox.hpp"

Mailbox::Mailbox(
    const glm::vec3& position,
    const glm::vec3& rotation,
    const glm::vec3& scale
)
:
    StaticObject(
        Assets::MAILBOX,
        position,
        rotation,
        scale
    ),
    m_basePosition(position)
{
}

void Mailbox::activate()
{
    if(m_activated)
        return;

    m_activated = true;
    m_time = 0.0f;
}

void Mailbox::update(float dt)
{
    if(!m_activated)
        return;

    m_time += dt;

    float bounce =
        sin(m_time * 12.0f) *
        exp(-m_time * 2.0f);

    setPosition(
        m_basePosition +
        glm::vec3(0.0f, bounce, 0.0f)
    );
}