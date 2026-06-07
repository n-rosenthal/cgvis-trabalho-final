#pragma once

#include "Objects/Interfaces/GameObject.hpp"
#include "Objects/Drawables/HouseDrawable.hpp"

class House : public GameObject
{
public:
    House(
        const glm::vec3& position,
        const glm::vec3& rotation,
        const glm::vec3& scale
    );
};