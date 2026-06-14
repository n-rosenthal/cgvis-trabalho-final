#pragma once

#include "Objects/Interfaces/GameObject.hpp"
#include "Objects/ObjDrawable.hpp"

#include "Bezier/BezierMover.hpp"

class ButterflyNPC : public GameObject
{
public:
    ButterflyNPC(
        const BezierPath& path
    );

    void update(float dt);

private:
    BezierMover m_mover;
};