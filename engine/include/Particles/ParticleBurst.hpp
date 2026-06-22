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
        float lifetime,
        const glm::vec4& color = glm::vec4(1.0f)
    );

    /**
     * @brief   Construtor vazio, para bursts alimentados aos
     *          poucos ao longo do tempo (ex: trails), em vez
     *          de todas as partículas de uma vez.
     */
    ParticleBurst() = default;

    /**
     * @brief   Adiciona uma única partícula ao burst, na posição
     *          e com a velocidade/cor/tempo de vida indicados.
     *          Usado por emissores contínuos (trails).
     */
    void addParticle(
        const glm::vec3& position,
        const glm::vec3& velocity,
        const glm::vec4& color,
        float size,
        float lifetime
    );

    void update(float dt);

    /**
     * @brief   Indica se o burst terminou e pode ser removido.
     *          Sempre falso se o burst estiver marcado como
     *          persistente (ver setPersistent()) — usado por
     *          trails, que ficam momentaneamente sem partículas
     *          entre uma emissão e outra, mas não "terminaram".
     */
    bool finished() const;

    /**
     * @brief   Marca o burst como persistente: nunca é considerado
     *          "finished()", mesmo com zero partículas vivas.
     *          Usado para bursts de trail, alimentados aos poucos
     *          via addParticle() e mantidos vivos indefinidamente.
     */
    void setPersistent(bool persistent) { m_persistent = persistent; }

    const std::vector<Particle>&
    particles() const;

private:
    std::vector<Particle> m_particles;
    bool m_persistent = false;
};