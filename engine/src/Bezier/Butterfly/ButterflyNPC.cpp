#include "Bezier/Butterfly/ButterflyNPC.hpp"

ButterflyNPC::ButterflyNPC(
    glm::vec3 position,
    glm::vec3 rotation,
    glm::vec3 scale
)
:
    GameObject(
        std::make_unique<ButterflyDrawable>(),
        position,
        rotation,
        scale
    ) {};

void ButterflyNPC::setPath(
    std::shared_ptr<BezierPath> path
)
{
    m_mover.setPath(*path);
}

void ButterflyNPC::update(float dt)
{
    m_mover.update(dt);

    m_position =
        m_mover.getPosition();

    glm::vec3 fwd =
        m_mover.getForward();

    float yaw =
        atan2(
            fwd.z,
            fwd.x
        );

    float pitch =
        atan2(
            fwd.y,
            sqrt(
                fwd.x*fwd.x +
                fwd.z*fwd.z
            )
        );

    m_rotation =
        glm::vec3(
            -pitch,
            yaw,
            0.0f
        );
}