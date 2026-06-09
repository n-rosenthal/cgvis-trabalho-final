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
    buildTerrain();
    m_bird.emplace();
    buildTrees();
    buildRocks();
    buildRings();
    buildLetter();
    buildHouses();
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
    if(m_letter &&
    !m_letter->isCaptured() &&
    !m_bird->carryingLetter())
    {
        float d =
            glm::length(
                m_letter->getPosition() -
                birdPos
            );

        if(d < radius + 2.0f)
        {
            m_bird->pickLetter(m_letter);

            g_Sound.play(
                "assets/audio/cartoon-boing-bouncy-big_F_major.wav"
            );
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

    if (m_bird->getStanding()) {
        r.drawBird(*m_bird, true);  // Passa true para usar modelo standing
    }   else {
        r.drawBird(*m_bird, false); // Passa false para usar modelo voando
    }

    r.drawTerrain(*m_terrain);
    r.drawRocks(m_rocks);
    r.drawRings(m_rings);
    if (m_letter) r.drawLetter(*m_letter);

    r.drawObjects(m_staticObjects);

    r.drawHouses(m_houses);
}


//  ======================================================================
//  Métodos `build*()` para construção dos objetos da cena virtual
//  ======================================================================
/**
 * @brief   Constrói o terreno do jogo
 */
void Scene::buildTerrain() {
    m_terrain = std::make_unique<Terrain>(
        128,
        128,
        5.0f
    );

    assert(m_terrain);
    m_terrain->generate();
}

/**
 * @brief   Constrói as rochas do jogo
 */
void Scene::buildRocks() {
    //  semente de aleatoriedade para disposição das rochas
    srand((unsigned int)time(nullptr));

    //  quantidade de rochas e região de distribuição
    const int   numRocks   = 40;
    const float regionSize = 256.0f;

    for (int i = 0; i < numRocks; ++i) {
        float x     = ((float)rand() / RAND_MAX - 0.5f) * regionSize;
        float z     = ((float)rand() / RAND_MAX - 0.5f) * regionSize;
        float scale = 0.8f + ((float)rand() / RAND_MAX) * 3.5f;
        float y     = m_terrain->getHeight(x, z);

        m_rocks.push_back(std::make_shared<ProceduralRock>(glm::vec3(x, y, z), scale, 2));
    };
};

void Scene::buildTrees()
{
    glm::vec3 lake_center(0.0f);
    const float lake_radius = 120.0f;

    const int numTrees = 20;
    const float borderDistance = 10.0f;

    srand((unsigned int)time(nullptr));

    for (int i = 0; i < numTrees; ++i)
    {
        float angle =
            ((float)rand() / RAND_MAX) *
            2.0f * M_PI;

        float radius =
            lake_radius + borderDistance;

        float x =
            lake_center.x +
            radius * cos(angle);

        float z =
            lake_center.z +
            radius * sin(angle);

        float height =
            m_terrain->getHeight(x, z);

        float yaw =
            ((float)rand() / RAND_MAX) *
            2.0f * M_PI;

        float scale =
            0.8f +
            ((float)rand() / RAND_MAX) * 3.5f;

        m_staticObjects.push_back(
            std::make_shared<StaticObject>(
                Assets::TREE_1,
                glm::vec3(x, height, z),
                glm::vec3(0.0f, yaw, 0.0f),
                glm::vec3(scale)
            )
        );
    }
}


/**
 * @brief   Constrói os anéis de objetivo do jogo
 */
void Scene::buildRings() {
    // Posições (x, offset_y, z) — offset_y é somado à altura do terreno
    const std::vector<glm::vec3> positions = {
        { 20.0f, 20.0f,  20.0f },
        { 40.0f, 30.0f,  25.0f },
        { 60.0f, 40.0f,  30.0f },
        { 80.0f, 50.0f,  25.0f },
        {100.0f, 60.0f,  20.0f },
    };

    //  construção dos anéis
    for (const auto& p : positions) {
        float groundY = m_terrain->getHeight(p.x, p.z);
        m_rings.push_back(std::make_shared<Ring>(glm::vec3(p.x, groundY + p.y, p.z)));
    };
};


/**
 *  @brief  construtor para a letter
 */
void Scene::buildLetter() {
    m_letter = std::make_shared<Letter>(glm::vec3(0.0f, 20.0f, 0.0f));
}

void Scene::buildHouses() {
    float x = 40.0f;
    float z = 50.0f;

    float y =
        m_terrain->getHeight(x,z);

    m_houses.push_back(
        std::make_shared<House>(
            glm::vec3(x,y,z),
            glm::vec3(0.0f),
            glm::vec3(8.0f)
        )
    );

    x = 80.0f;
    z = 110.0f;
    y =
        m_terrain->getHeight(x,z);

    m_houses.push_back(
        std::make_shared<House>(
            glm::vec3(x,y,z),
            glm::vec3(glm::radians(-90.0f), 0.0f, 0.0f),
            glm::vec3(8.0f)
        )
    );
}