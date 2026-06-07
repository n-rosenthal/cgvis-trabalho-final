/**
 * @file    `Scene.cpp`
 * @brief   Implementação da classe `Scene`
 */

#include <glad/glad.h>
#include "Game/Scene.hpp"
#include "Game/Renderer.hpp"
#include "audio/AudioManager.hpp"
#include "Collision/CollisionSystem.hpp"


extern SoundManager g_Sound;

// distancePointSegment — mantida aqui, fora do main
static float distancePointSegment(glm::vec3 p, glm::vec3 a, glm::vec3 b) {
    glm::vec3 ab = b - a;
    float t = glm::dot(p - a, ab) / glm::dot(ab, ab);
    t = glm::clamp(t, 0.0f, 1.0f);
    return glm::length(p - (a + t * ab));
}


/**
 * @brief   Cria os objetos do jogo
 */
void Scene::build() {
    m_terrain = m_builder.buildTerrain();

    m_bird.emplace();

    m_trees = m_builder.buildTrees(*m_terrain);
    m_rocks = m_builder.buildRocks(*m_terrain);
    m_rings = m_builder.buildRings(*m_terrain);
    m_letter = m_builder.buildLetter();
}

/**
 * @brief   Atualiza os objetos do jogo
 * 
 * @param   dt  (float)
 *              passagem do tempo
 * @param   w   (GLFWwindow*)
 *              ponteiro para a janela 
 */
void Scene::update(float dt, GLFWwindow* w) {
    m_bird->update(dt, w);
    m_camera.update(m_bird->getPosition(),
                    m_bird->getForward(),
                    m_bird->getUp(), dt);

    if (m_letter) {
        m_letter->update(dt, m_bird->getPosition(), m_bird->getRotation(), m_letter->isCaptured());
    }

    for (auto& ring : m_rings)
        ring->update(dt);
}


/**
 * @brief       Resolve as colisões do jogo
 */
void Scene::resolveCollisions() {
    const glm::vec3 birdPos  = m_bird->getPosition();
    const glm::vec3 capsuleA = m_bird->getCollider().p0;
    const glm::vec3 capsuleB = m_bird->getCollider().p1;
    const float     radius   = m_bird->getCollider().radius;

    // Terreno
    const float th = m_terrain->getHeight(birdPos.x, birdPos.z);
    if (birdPos.y - 0.6f <= th) {
        glm::vec3 normal = m_terrain->getNormal(birdPos.x, birdPos.z);
        if (m_bird->onTerrainCollision(th, normal))
            g_Sound.play("assets/audio/cartoon-boing-bouncy-big_F_major.wav");
    }

    // Rochas
    for (auto& rock : m_rocks) {
        float d = distancePointSegment(rock->getPosition(), capsuleA, capsuleB);
        if (d < radius + rock->getCollisionRadius()) {
            g_Sound.play("assets/audio/cartoonish-stone-sfx-slow.wav");
            m_bird->onCollision(rock->getPosition());
        }
    }

    // Carta
    if (m_letter && !m_letter->isCaptured()) {
        float d = glm::length(m_letter->getPosition() - birdPos);
        if (d < radius + 2.0f) { // Capture radius
            m_letter->setCaptured(true);
            g_Sound.play("assets/audio/cartoon-boing-bouncy-big_F_major.wav");
        }
    }

    // Anéis — coleta e remove os mortos
    for (auto& ring : m_rings)
        ring->checkCollision(birdPos);

    m_rings.erase(
        std::remove_if(m_rings.begin(), m_rings.end(),
            [](const std::shared_ptr<Ring>& r){ return r->isDead(); }),
        m_rings.end()
    );
}



/**
 * @brief   Invoca o renderizador para desenhar os objetos propriamente
 * @param   r   (Renderer&)
 *              renderizador
 *  
 */
void Scene::draw(Renderer& r) {
    r.drawBird(*m_bird);
    r.drawTerrain(*m_terrain);
    r.drawRocks(m_rocks);
    r.drawTrees(m_trees);
    r.drawRings(m_rings);
    if (m_letter) r.drawLetter(*m_letter);
}
