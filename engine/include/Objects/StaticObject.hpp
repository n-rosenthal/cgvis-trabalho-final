#pragma once

#include "Objects/Interfaces/GameObject.hpp"
#include "Objects/Interfaces/Collidable.hpp"
#include "Objects/ObjDrawable.hpp"
#include "Objects/Assets.hpp"

#include "Collision/Collider.hpp"
#include <memory>
#include <vector>

//  Análago ao `ColliderTag`
//  utilizado para identificar os colisores
enum class StaticObjectType {
    //  Árvores
    OAK,

    //  Arbustos
    SHRUB,

    //  Arbustos
    BUSH,
    MARINE_PLANT,
    
    //  Pedras
    ROCK_2,
    ROCK_4,
    ROCK_5,
    ROCK_6,
    ROCK_7,
    ROCK_8,
    ROCK_9,

    //  Outros
    MAILBOX,
    HOUSE,
    LETTER
};


class StaticObject : public GameObject, public Collidable {
public:
    // Construtor principal (múltiplos colliders)
    StaticObject(
        const ModelDefinition& model,
        const glm::vec3& position,
        const glm::vec3& rotation,
        const glm::vec3& scale,
        StaticObjectType type,                           // <-- novo
        std::vector<std::shared_ptr<Collider>> colliders = {}
    );

    // Construtor com um único collider
    StaticObject(
        const ModelDefinition& model,
        const glm::vec3& position,
        const glm::vec3& rotation,
        const glm::vec3& scale,
        StaticObjectType type,                           // <-- novo
        std::shared_ptr<Collider> collider
    );

    // ---- Interface Collidable ----
    std::vector<std::shared_ptr<Collider>> getColliders() const override;
    void updateColliders() override;
    void onCollision(glm::vec3 objectPosition) override;
    StaticObjectType getType() const { return m_type; }

private:
    std::vector<std::shared_ptr<Collider>> m_colliders;
    StaticObjectType m_type;
};