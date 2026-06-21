#include "Bezier/BezierPath.hpp"

void BezierPath::addPoint(glm::vec3 point)
{
    m_points.push_back(point);
}

glm::vec3 BezierPath::evaluate(float t) const
{
    if (m_points.size() < 4)
        return glm::vec3(0.0f);

    int numSegments =
        (m_points.size() - 1) / 3;

    t = glm::clamp(t, 0.0f, 1.0f);

    float scaledT =
        t * numSegments;

    int segment =
        std::min(
            (int)scaledT,
            numSegments - 1
        );

    float localT =
        scaledT - segment;

    int i = segment * 3;

    const glm::vec3& p0 = m_points[i + 0];
    const glm::vec3& p1 = m_points[i + 1];
    const glm::vec3& p2 = m_points[i + 2];
    const glm::vec3& p3 = m_points[i + 3];

    float u = 1.0f - localT;

    return
        u*u*u*p0 +
        3.0f*u*u*localT*p1 +
        3.0f*u*localT*localT*p2 +
        localT*localT*localT*p3;
}

glm::vec3 BezierPath::tangent(float t) const
{
    if (m_points.size() < 4)
        return glm::vec3(1,0,0);

    int numSegments =
        (m_points.size() - 1) / 3;

    t = glm::clamp(t, 0.0f, 1.0f);

    float scaledT =
        t * numSegments;

    int segment =
        std::min(
            (int)scaledT,
            numSegments - 1
        );

    float localT =
        scaledT - segment;

    int i = segment * 3;

    const glm::vec3& p0 = m_points[i + 0];
    const glm::vec3& p1 = m_points[i + 1];
    const glm::vec3& p2 = m_points[i + 2];
    const glm::vec3& p3 = m_points[i + 3];

    float u = 1.0f - localT;

    glm::vec3 d =
        3.0f*u*u*(p1-p0)
        +
        6.0f*u*localT*(p2-p1)
        +
        3.0f*localT*localT*(p3-p2);

    return glm::normalize(d);
}