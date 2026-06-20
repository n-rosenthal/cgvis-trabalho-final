#include "Particles/TrailEmitter.hpp"

// RandomFloat já é definida em ParticleBurst.cpp
float RandomFloat(float a, float b);

TrailEmitter::TrailEmitter(
    float emitInterval,
    float particleLifetime,
    float particleSize,
    const glm::vec4& color
)
    : m_emitInterval(emitInterval)
    , m_particleLifetime(particleLifetime)
    , m_particleSize(particleSize)
    , m_color(color)
{
}

void TrailEmitter::update(float dt)
{
    m_timeSinceLastEmit += dt;
}

void TrailEmitter::emitAt(
    ParticleBurst& target,
    const glm::vec3& position,
    float spread
)
{
    if (m_timeSinceLastEmit < m_emitInterval)
        return;

    m_timeSinceLastEmit = 0.0f;

    glm::vec3 velocity(
        RandomFloat(-spread, spread),
        RandomFloat(-spread, spread),
        RandomFloat(-spread, spread)
    );

    target.addParticle(
        position,
        velocity,
        m_color,
        m_particleSize,
        m_particleLifetime
    );
}