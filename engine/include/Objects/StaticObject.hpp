#pragma once
#include "Objects/Interfaces/GameObject.hpp"
#include <string>



class StaticObject : public GameObject
{
public:
    StaticObject(
        std::unique_ptr<Drawable> drawable,
        const glm::vec3& position,
        const glm::vec3& rotation,
        const glm::vec3& scale
    );

    StaticObject(
        const std::vector<std::string>& models,
        const glm::vec3& position,
        const glm::vec3& rotation,
        const glm::vec3& scale
    );
};