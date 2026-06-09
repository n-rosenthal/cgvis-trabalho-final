#pragma once

#include "Objects/Interfaces/GameObject.hpp"
#include "Objects/ObjDrawable.hpp"
#include "Objects/Assets.hpp"

class StaticObject : public GameObject {
public:
    StaticObject(
        const ModelDefinition& model,
        const glm::vec3& position,
        const glm::vec3& rotation,
        const glm::vec3& scale
    );
};