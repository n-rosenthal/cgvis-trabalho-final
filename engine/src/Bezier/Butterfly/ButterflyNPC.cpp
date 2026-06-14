#include "Bezier/Butterfly/ButterflyNPC.hpp"

ButterflyNPC::ButterflyNPC(
    const BezierPath& path
)
:
GameObject(
    std::make_unique<ObjDrawable>(
        Assets::BUTTERFLY
    ),
    glm::vec3(0.0f),
    glm::vec3(0.0f),
    glm::vec3(1.0f)
),
m_mover(path)
{
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