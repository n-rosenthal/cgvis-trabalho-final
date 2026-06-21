#pragma once

#include "Bezier/PathFollower.hpp"
#include "Objects/Interfaces/GameObject.hpp"
#include "Bezier/Butterfly/ButterflyDrawable.hpp"

class ButterflyNPC :
    public GameObject,
    public PathFollower
{
public:
    ButterflyNPC(
        std::shared_ptr<BezierPath> path,
        glm::vec3 position,
        glm::vec3 rotation,
        glm::vec3 scale
    )
    :
        PathFollower(path),
        GameObject(
            std::make_unique<ButterflyDrawable>(),
            position,
            rotation,
            scale
        )
        {};

    void update(float dt) override;
};