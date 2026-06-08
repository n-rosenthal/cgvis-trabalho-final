#include "Objects/StaticObject.hpp"
#include "Objects/ObjDrawable.hpp"

StaticObject::StaticObject(
    std::unique_ptr<Drawable> drawable,
    const glm::vec3& position,
    const glm::vec3& rotation,
    const glm::vec3& scale
)
:
GameObject(
    std::move(drawable),
    position,
    rotation,
    scale
)
{
}

StaticObject::StaticObject(
    const std::vector<std::string>& models,
    const glm::vec3& position,
    const glm::vec3& rotation,
    const glm::vec3& scale
)
:
GameObject(
    std::make_unique<ObjDrawable>(models),
    position,
    rotation,
    scale
)
{
}