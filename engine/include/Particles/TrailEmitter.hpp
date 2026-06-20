#pragma once

#include <glm/glm.hpp>
#include "Particles/ParticleBurst.hpp"

/**
 * @file    `TrailEmitter.hpp`
 * @brief   Emissor contínuo de partículas, usado para criar rastros
 *          (trails) atrás de objetos em movimento — ex: o pássaro
 *          voando, ou a carta arremessada.
 *
 * @details Diferente de um `ParticleBurst` usado isoladamente (uma
 *          explosão única, ex: coletar anel), o `TrailEmitter` é
 *          atualizado a cada frame e decide, com base num intervalo
 *          configurável, se deve nascer uma nova partícula na
 *          posição atual do objeto que ele segue. As partículas
 *          são escritas diretamente num `ParticleBurst` já existente
 *          (ex: um dos bursts mantidos por `Scene`), para que o
 *          trail seja desenhado pelo mesmo `Renderer::drawParticles`
 *          já usado para os demais efeitos, sem nenhuma mudança no
 *          Renderer.
 */
class TrailEmitter
{
public:
    /**
     * @brief   Construtor do emissor de trail
     *
     * @param   emitInterval (float)
     *          intervalo em segundos entre cada partícula emitida
     * @param   particleLifetime (float)
     *          tempo de vida de cada partícula do trail
     * @param   particleSize (float)
     *          tamanho de cada partícula do trail
     * @param   color (glm::vec4)
     *          cor (RGBA) das partículas emitidas
     */
    TrailEmitter(
        float emitInterval,
        float particleLifetime,
        float particleSize,
        const glm::vec4& color
    );

    /**
     * @brief   Avança o tempo do emissor (apenas o timer, não as
     *          partículas — estas são atualizadas pelo
     *          `ParticleBurst` de destino, junto com os demais
     *          bursts da cena).
     *
     * @param   dt (float)
     *          passagem do tempo
     */
    void update(float dt);

    /**
     * @brief   Marca que o objeto seguido está na posição `position`
     *          neste frame. Se o intervalo de emissão já decorreu,
     *          nasce uma nova partícula ali (escrita em `target`),
     *          com velocidade pequena espalhada aleatoriamente.
     *
     * @param   target (ParticleBurst&)
     *          burst de destino onde a partícula será adicionada
     * @param   position (glm::vec3)
     *          posição atual do objeto (ex: pássaro, carta)
     * @param   spread (float)
     *          magnitude da velocidade aleatória de espalhamento
     *          (default: pequeno, para um rastro discreto)
     */
    void emitAt(
        ParticleBurst& target,
        const glm::vec3& position,
        float spread = 0.5f
    );

private:
    float m_emitInterval;
    float m_particleLifetime;
    float m_particleSize;
    glm::vec4 m_color;

    float m_timeSinceLastEmit = 0.0f;
};