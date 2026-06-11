// House.hpp
#pragma once
#include "Objects/Interfaces/GameObject.hpp"
#include "Objects/Drawables/HouseDrawable.hpp"

class House : public GameObject {
public:
    explicit House(const glm::vec3& position)
        : GameObject(
            std::make_unique<HouseDrawable>(),
            position,           // posição
            glm::vec3(0.0f),    // rotação
            glm::vec3(1.0f)     // escala
          )
    {}
};