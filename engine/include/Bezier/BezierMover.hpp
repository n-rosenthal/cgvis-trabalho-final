#pragma once

#include "Bezier/BezierPath.hpp"
#include <glm/glm.hpp>
#include <memory>

class BezierMover
{
public:
    void update(float dt);

    glm::vec3 getForward() const;
    glm::vec3 getPosition() const;
    void setPath(const BezierPath& path) { m_path = path; };

private:
    BezierPath m_path;

    float m_t = 0.0f;

    float m_speed = 0.1f;
};