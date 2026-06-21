#include "Objects/Mailbox.hpp"
#include "Collision/AABBCollider.hpp"   // ou o tipo de collider que desejar

Mailbox::Mailbox(
    const glm::vec3& position,
    const glm::vec3& rotation,
    const glm::vec3& scale
)
:
    StaticObject(
        Assets::MAILBOX,                     // model
        position,                            // position
        rotation,                            // rotation
        scale,                               // scale
        StaticObjectType::MAILBOX,           // type
        std::vector<std::shared_ptr<Collider>>{
            std::make_shared<AABBCollider>(  // collider
                position - glm::vec3(1.0f, 0.0f, 0.8f),
                position + glm::vec3(1.0f, 2.0f, 0.8f)
            )
        }
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