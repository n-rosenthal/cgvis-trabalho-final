#pragma once

#include <glm/glm.hpp>
#include <vector>

class BezierPath
{
public:
    void addPoint(glm::vec3 p);

    glm::vec3 evaluate(float t) const;
    glm::vec3 tangent(float t) const;
private:
    std::vector<glm::vec3> m_points;
};