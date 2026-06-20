#include "Particles/ParticleBurst.hpp"

float RandomFloat(float a, float b) {
    float random = ((float) rand()) / (float) RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return a + r;
};

const std::vector<Particle>& ParticleBurst::particles() const
{
    return m_particles;
};

void ParticleBurst::addParticle(
    const glm::vec3& position,
    const glm::vec3& velocity,
    const glm::vec4& color,
    float size,
    float lifetime
)
{
    Particle p;

    p.position = position;
    p.velocity = velocity;

    p.life    = lifetime;
    p.maxLife = lifetime;

    p.size  = size;
    p.color = color;

    m_particles.push_back(p);
}

ParticleBurst::ParticleBurst(
    const glm::vec3& origin,
    size_t count,
    float speed,
    float lifetime
)
{
    m_particles.reserve(count);

    for(size_t i=0;i<count;i++)
    {
        glm::vec3 dir;

        while(glm::length(dir) < 0.0001f)
        {
            dir = glm::vec3(
                RandomFloat(-1.f,1.f),
                RandomFloat(-1.f,1.f),
                RandomFloat(-1.f,1.f)
            );
        }

        dir = glm::normalize(dir);

        Particle p;

        p.position = origin;
        p.velocity = dir * speed;

        p.life = lifetime;
        p.maxLife = lifetime;

        p.size = 0.1f;

        p.color =
            glm::vec4(
                1.f,
                0.8f,
                0.2f,
                1.f
            );

        m_particles.push_back(p);
    }
};

void ParticleBurst::update(float dt)
{
    for(auto& p : m_particles)
    {
        p.life -= dt;

        if(p.life <= 0.0f)
            continue;

        p.position += p.velocity * dt;
        p.velocity.y -= 9.8f * dt;

        p.color.a = p.life / p.maxLife;
    }

    m_particles.erase(
        std::remove_if(
            m_particles.begin(),
            m_particles.end(),
            [](const Particle& p)
            {
                return p.life <= 0.0f;
            }
        ),
        m_particles.end()
    );
}

bool ParticleBurst::finished() const
{
    if(m_persistent)
        return false;

    for(const auto& p : m_particles)
    {
        if(p.life > 0.0f)
            return false;
    }

    return true;
}