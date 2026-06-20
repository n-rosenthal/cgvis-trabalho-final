#pragma once

#include <vector>
#include <algorithm>
#include "Particles/Particle.hpp"

class ParticleBurst
{
public:
    ParticleBurst(
        const glm::vec3& origin,
        size_t count,
        float speed,
        float lifetime
    );

    void update(float dt);

    bool finished() const;

    const std::vector<Particle>&
    particles() const;

private:
    std::vector<Particle> m_particles;
};