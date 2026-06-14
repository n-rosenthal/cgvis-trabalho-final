#pragma once

#include "Objects/Interfaces/GameObject.hpp"
#include "Objects/ObjDrawable.hpp"

#include "Bezier/BezierMover.hpp"
#include "Bezier/Butterfly/ButterflyDrawable.hpp"

class ButterflyNPC : public GameObject
{
public:
    ButterflyNPC(
        glm::vec3 position,
        glm::vec3 rotation,
        glm::vec3 scale
    );

    void setPath(
        std::shared_ptr<BezierPath> path
    );
    void update(float dt);

private:
    BezierMover m_mover;
};