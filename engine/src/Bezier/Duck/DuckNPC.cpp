#include "Bezier/Duck/DuckNPC.hpp"

void DuckNPC::update(float dt)
{
    // Atualiza a posição ao longo do caminho
    PathFollower::update(dt);

    // Obtém posição e direção do caminho
    m_position = getPathPosition();
    glm::vec3 fwd = getPathForward();

    // Calcula os ângulos de rotação (yaw e pitch) a partir do vetor forward
    float yaw   = atan2(fwd.z, fwd.x);
    float pitch = atan2(fwd.y, sqrt(fwd.x * fwd.x + fwd.z * fwd.z));

    m_rotation = glm::vec3(-pitch, yaw, 0.0f);
}