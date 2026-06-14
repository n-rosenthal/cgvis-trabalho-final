#include "Bezier/BezierMover.hpp"

void BezierMover::update(float dt)
{
    m_t += m_speed * dt;

    if(m_t > 1.0f)
        m_t -= 1.0f;
}

glm::vec3 BezierMover::getPosition() const
{
    return m_path.evaluate(m_t);
}

glm::vec3 BezierMover::getForward() const
{
    return
        m_path.tangent(
            m_t
        );
}