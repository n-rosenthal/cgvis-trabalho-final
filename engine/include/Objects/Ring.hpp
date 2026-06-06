/**
 * @file    Ring.hpp
 * @brief   Anel de objetivo como GameObject.
 *          Lógica de coleta/animação aqui;
 *          geometria e draw no RingDrawable.
 */
#pragma once

#include "Objects/Interfaces/GameObject.hpp"
#include "Objects/Drawables/RingDrawable.hpp"

class Ring : public GameObject {
public:
    explicit Ring(glm::vec3 position, float radius = 2.5f);

    void update(float dt);
    bool checkCollision(glm::vec3 birdPos);
    bool isDead() const;

    // Render override: injeta view e estado de animação antes de delegar
    void render(const DrawContext& ctx, const glm::mat4& view);

private:
    float m_radius;
    float m_pulseTime   = 0.0f;
    float m_animScale   = 1.0f;
    float m_destroyTimer = 0.0f;
    bool  m_collected   = false;

    RingDrawable* m_ringDrawable = nullptr;   // alias sem ownership (dono é m_drawable)
};