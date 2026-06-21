#pragma once

#include "Objects/Interfaces/GameObject.hpp"
#include "Objects/Interfaces/Collidable.hpp"

#include "Objects/Drawables/RingDrawable.hpp"

#include "Collision/SphereCollider.hpp"

class Ring :
    public GameObject,
    public Collidable
{
public:
    explicit Ring(
        glm::vec3 position,
        float radius = 2.5f
    );

    void update(float dt);

    bool isDead() const;

    void collect();
    bool collected() const { return m_collected; }
    void render(
        const DrawContext& ctx,
        const glm::mat4& view
    );

    // -----------------------
    // Collidable
    // -----------------------
    std::vector<std::shared_ptr<Collider>> getColliders() const override;
    void updateColliders() override;
    void onCollision(glm::vec3 objectPosition) override;

    glm::vec3 getPosition() const { return m_position; }

    float getRadius() const { return m_radius; }

private:
    float m_radius;

    float m_pulseTime     = 0.0f;
    float m_animScale     = 1.0f;
    float m_destroyTimer  = 0.0f;

    bool m_collected      = false;

    RingDrawable* m_ringDrawable = nullptr;

    std::shared_ptr<SphereCollider> m_collider;
};