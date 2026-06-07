#include "Objects/House.hpp"

House::House(
    const glm::vec3& position,
    const glm::vec3& rotation,
    const glm::vec3& scale
)
:
GameObject(
    std::make_unique<HouseDrawable>(),
    position,
    rotation,
    scale
)
{
}