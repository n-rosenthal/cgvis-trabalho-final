#include "Objects/StaticObject.hpp"

StaticObject::StaticObject(
    const ModelDefinition& model,
    const glm::vec3& position,
    const glm::vec3& rotation,
    const glm::vec3& scale
)
:
GameObject(
    std::make_unique<ObjDrawable>(model),
    position,
    rotation,
    scale
)
{
}