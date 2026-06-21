#pragma once

#include <glm/glm.hpp>
#include <memory>

#include "Bezier/BezierMover.hpp"
#include "Bezier/BezierPath.hpp"

class PathFollower
{
public:
    explicit PathFollower(
        std::shared_ptr<BezierPath> path
    )
        : m_path(path)
    {
        m_mover.setPath(*path);
    }

    virtual ~PathFollower() = default;

    virtual void update(float dt)
    {
        m_mover.update(dt);
    }

    void setPath(std::shared_ptr<BezierPath> path)
    {
        m_path = path;

        if (m_path)
            m_mover.setPath(*m_path);
    }

    glm::vec3 getPathPosition() const
    {
        return m_mover.getPosition();
    }

    glm::vec3 getPathForward() const
    {
        return m_mover.getForward();
    }

protected:
    std::shared_ptr<BezierPath> m_path;
    BezierMover m_mover;
};