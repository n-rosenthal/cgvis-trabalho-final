#include "Bezier/Butterfly/ButterflyNPC.hpp"

void ButterflyNPC::update(float dt)
{
    PathFollower::update(dt);

    m_position =
        getPathPosition();

    glm::vec3 fwd =
        getPathForward();

    float yaw =
        atan2(
            fwd.z,
            fwd.x
        );

    float pitch =
        atan2(
            fwd.y,
            sqrt(
                fwd.x * fwd.x +
                fwd.z * fwd.z
            )
        );

    m_rotation =
        glm::vec3(
            -pitch,
            yaw,
            0.0f
        );
}