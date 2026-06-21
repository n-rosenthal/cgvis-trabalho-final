#include "Bezier/BezierPath.hpp"

void BezierPath::addPoint(glm::vec3 point)
{
    m_points.push_back(point);
}

glm::vec3 BezierPath::evaluate(float t) const
{
    if (m_points.empty())
        return glm::vec3(0.0f);

    auto idx = [&](int i)
    {
        int n = static_cast<int>(m_points.size());

        while (i < 0)
            i += n;

        return i % n;
    };

    int segmentCount = static_cast<int>(m_points.size()) - 1;

    // curva fechada
    float globalT = glm::clamp(t, 0.0f, 1.0f) * segmentCount;

    int seg = std::min(
        static_cast<int>(globalT),
        segmentCount - 1
    );

    float localT = globalT - seg;

    const glm::vec3& P1 = m_points[seg];
    const glm::vec3& P2 = m_points[seg + 1];

    glm::vec3 P0 =
        (seg == 0)
            ? P1
            : m_points[seg - 1];

    glm::vec3 P3 =
        (seg + 2 >= m_points.size())
            ? P2
            : m_points[seg + 2];

    // controles automáticos
    glm::vec3 C1 =
        P1 + (P2 - P0) / 6.0f;

    glm::vec3 C2 =
        P2 - (P3 - P1) / 6.0f;

    float u = 1.0f - localT;

    return
          u*u*u * P1
        + 3.0f*u*u*localT * C1
        + 3.0f*u*localT*localT * C2
        + localT*localT*localT * P2;
}

glm::vec3 BezierPath::tangent(float t) const
{
    if (m_points.size() < 2)
        return glm::vec3(1.0f, 0.0f, 0.0f);

    auto idx = [&](int i)
    {
        int n = static_cast<int>(m_points.size());

        while (i < 0)
            i += n;

        return i % n;
    };

    int segmentCount = static_cast<int>(m_points.size()) - 1;

    float globalT =
        glm::clamp(t, 0.0f, 1.0f) *
        segmentCount;

    int seg = std::min(
        static_cast<int>(globalT),
        segmentCount - 1
    );

    float localT = globalT - seg;

    const glm::vec3& P1 = m_points[seg];
    const glm::vec3& P2 = m_points[seg + 1];

    glm::vec3 P0 =
        (seg == 0)
            ? P1
            : m_points[seg - 1];

    glm::vec3 P3 =
        (seg + 2 >= m_points.size())
            ? P2
            : m_points[seg + 2];

    glm::vec3 C1 =
        P1 + (P2 - P0) / 6.0f;

    glm::vec3 C2 =
        P2 - (P3 - P1) / 6.0f;

    float u = 1.0f - localT;

    glm::vec3 d =
          3.0f*u*u*(C1 - P1)
        + 6.0f*u*localT*(C2 - C1)
        + 3.0f*localT*localT*(P2 - C2);

    return glm::normalize(d);
}