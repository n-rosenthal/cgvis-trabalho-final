#pragma once

#include "Bezier/Duck/DuckDrawable.hpp"
#include "Bezier/PathFollower.hpp"  
#include "Objects/Interfaces/GameObject.hpp"

class DuckNPC :
    public GameObject,
    public PathFollower
{
public:
    DuckNPC(
        std::shared_ptr<BezierPath> path,
        glm::vec3 position,
        glm::vec3 rotation,
        glm::vec3 scale
    )
    :
        PathFollower(path),
        GameObject(
            std::make_unique<DuckDrawable>(),
            position,
            rotation,
            scale
        ) {};

    

    // Sobrescreve apenas o update para atualizar rotação com base no caminho
    void update(float dt) override;
};