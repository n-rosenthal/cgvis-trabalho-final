#include "Bezier/Duck/DuckNPC.hpp"

void DuckNPC::update(float dt)
{
    PathFollower::update(dt);

    m_position = getPathPosition();
    glm::vec3 fwd = getPathForward();

    if (glm::length(fwd) < 0.001f) return;

    // Corrige o yaw: alinha o eixo +Z do modelo com a direção do movimento
    float yaw = atan2(fwd.x, fwd.z);   // ← principal alteração
    yaw += glm::pi<float>();

    // Se ainda estiver de costas, descomente a linha abaixo:
    yaw += glm::pi<float>();

    float pitch = atan2(fwd.y, sqrt(fwd.x*fwd.x + fwd.z*fwd.z));

    m_rotation = glm::vec3(-pitch, yaw, 0.0f);
}