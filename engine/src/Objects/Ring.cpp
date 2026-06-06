/**
 * @file    Ring.cpp
 */
#include "Objects/Ring.hpp"
#include "audio/AudioManager.hpp"
#include <cmath>

extern SoundManager g_Sound;

Ring::Ring(glm::vec3 position, float radius)
    : GameObject(std::make_unique<RingDrawable>(radius),
                 position,
                 glm::vec3(0.0f),
                 glm::vec3(1.0f))
    , m_radius(radius)
{
    // Guarda alias tipado para poder chamar setViewMatrix/setPulseTime
    m_ringDrawable = static_cast<RingDrawable*>(m_drawable.get());
    m_drawable->generate();
}

void Ring::update(float dt) {
    m_pulseTime += dt;

    if (m_collected) {
        m_destroyTimer += dt;
        m_animScale    += dt * 3.0f;
    }

    // Sincroniza estado de animação com o Drawable
    m_ringDrawable->setPulseTime(m_pulseTime);
    m_ringDrawable->setAnimScale(m_animScale);
}

bool Ring::checkCollision(glm::vec3 birdPos) {
    if (m_collected) return false;

    glm::vec3 delta = birdPos - m_position;
    float horizDist = sqrt(delta.x * delta.x + delta.z * delta.z);

    if (horizDist < m_radius && fabs(delta.y) < 2.0f) {
        m_collected = true;
        g_Sound.play("assets/audio/drum-roll-and-bell_112bpm.wav");
        return true;
    }
    return false;
}

bool Ring::isDead() const {
    return m_destroyTimer > 1.0f;
}

void Ring::render(const DrawContext& ctx, const glm::mat4& view) {
    m_ringDrawable->setViewMatrix(view);
    GameObject::render(ctx);   // chama Drawable::model -> draw
}