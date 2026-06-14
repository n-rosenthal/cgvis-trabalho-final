#include "Bezier/BezierPath.hpp"

glm::vec3 BezierPath::evaluate(float t) const
{
    const glm::vec3& p0 = m_points[0];
    const glm::vec3& p1 = m_points[1];
    const glm::vec3& p2 = m_points[2];
    const glm::vec3& p3 = m_points[3];

    float u = 1.0f - t;

    return
        u*u*u*p0 +
        3.0f*u*u*t*p1 +
        3.0f*u*t*t*p2 +
        t*t*t*p3;
}

glm::vec3 BezierPath::tangent(float t) const {
    const glm::vec3& p0 = m_points[0];
    const glm::vec3& p1 = m_points[1];
    const glm::vec3& p2 = m_points[2];
    const glm::vec3& p3 = m_points[3];

    float u = 1.0f - t;

    return glm::normalize(
        3.0f*u*u*(p1-p0)
        + 6.0f*u*t*(p2-p1)
        + 3.0f*t*t*(p3-p2)
    );
}