/**
 * @file    `Scene.cpp`
 * @brief   Implementação da classe `Scene`
 */

#include <glad/glad.h>
#include "Game/Scene.hpp"
#include "Game/Renderer.hpp"
#include "audio/AudioManager.hpp"
#include "Collision/CollisionSystem.hpp"

#include "Game/Window.hpp" // varr. depuração LETTER

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
    //  Terreno
    buildTerrain();

    //  Pássaro
    m_bird.emplace();
    m_bird->setPosition(glm::vec3(-180.0f, m_terrain->getHeight(-180.0f, -200.0f) + 20.0f, -200.0f));

    //  Objetos estáticos
    buildStaticObjects();

    //  Anéis
    buildRings();

    //  Carta
    buildLetter();


    //  Mailbox
    buildMailbox();

    //  NPCs
    buildButterflyNPCs();
    buildCarpNPCs();
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
    
    //  depuração de LETTER
    g_DebugLetterPosition = m_letter->getPosition();
    g_DebugLetterRotation = m_letter->getRotation();
    
    // --------------------------------------------------
    // Soltar carta (tecla G)
    // --------------------------------------------------

    static bool gHeld = false;

    bool gPressed =
        glfwGetKey(
            w,
            GLFW_KEY_G
        ) == GLFW_PRESS;

    if(gPressed && !gHeld) {
        if(m_letterState == LetterState::Carried) {
            m_letterState =
                LetterState::Falling;

            glm::vec3 dropPos =
                m_bird->getPosition()
                -
                m_bird->getForward() * 2.0f;

            m_letter->setPosition(dropPos);

            m_letterVelocity =
                m_bird->getForward() * 5.0f;
        }
    }

    gHeld = gPressed;

    // --------------------------------------------------

    updateLetter(dt, w);

    m_camera.update(
        m_bird->getPosition(),
        m_bird->getForward(),
        m_bird->getUp(),
        dt
    );

    for(auto& ring : m_rings) {
        ring->update(dt);
    }


    // NPCs

    for(auto& npc : m_butterflyNPCs) {
        npc->update(dt);
    }

    for(auto& carp : m_carpNPCs) {
        carp->update(dt);
    }
}

void Scene::updateLetter(
    float dt,
    GLFWwindow* window
)
{
    if(!m_letter)
        return;

    switch(m_letterState)
    {
        case LetterState::Carried:
        {
            glm::vec3 offset =
                m_bird->getForward() * 2.0f
                -
                m_bird->getUp() * 0.5f;

            m_letter->setPosition(
                m_bird->getPosition()
                +
                offset
            );

            m_letter->setRotation(
                m_bird->getRotation()
            );

            break;
        }

        case LetterState::Falling:
        {
            m_letterVelocity.y -=
                9.8f * dt;

            glm::vec3 p =
                m_letter->getPosition();

            p +=
                m_letterVelocity * dt;

            m_letter->setPosition(p);

            break;
        }

        case LetterState::OnGround:
        {
            break;
        }
    }
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

    // --------------------------------------------------
    // Carta no chão → pode ser capturada
    // --------------------------------------------------

    if(
        m_letter &&
        m_letterState == LetterState::OnGround
    )
    {
        float d =
            glm::length(
                m_letter->getPosition()
                -
                birdPos
            );

        if(d < radius + 2.0f)
        {
            m_letterState =
                LetterState::Carried;

            m_letterVelocity =
                glm::vec3(0.0f);

            g_Sound.play(
                "assets/audio/cartoon-boing-bouncy-big_F_major.wav"
            );
        }
    }

    if(
    m_letter &&
    m_letterState == LetterState::Falling
    )
    {
        glm::vec3 p =
            m_letter->getPosition();

        float terrainHeight =
            m_terrain->getHeight(
                p.x,
                p.z
            );

        float letterRadius =
            m_letter->getSize();

        if(
            p.y <= terrainHeight + letterRadius
        )
        {
            p.y =
                terrainHeight +
                letterRadius;

            m_letter->setPosition(p);

            m_letterVelocity =
                glm::vec3(0.0f);

            m_letterState =
                LetterState::OnGround;

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
    // r.drawRocks(m_rocks);
    r.drawRings(m_rings);
    if (m_letter) r.drawLetter(*m_letter);

    r.drawMailbox(*m_mailbox);

    r.drawObjects(m_staticObjects);

    // npcs
    for(auto& npc : m_butterflyNPCs) {
        r.drawButterflyNPC(*npc);
    }

    // for(auto& npc : m_carpNPCs) {
    //     r.drawCarpNPC(*npc);
    // }
}


//  ======================================================================
//  Métodos `build*()` para construção dos objetos da cena virtual
//  ======================================================================
/**
 * @brief   Constrói o terreno do jogo
 */
void Scene::buildTerrain() {
    m_terrain = std::make_unique<Terrain>(
        130.5,
        90.5,
        6.0f
    );

    assert(m_terrain);
    m_terrain->generate();
}


/**
 * @brief   Constrói os objetos estáticos do jogo
 */
void Scene::buildStaticObjects() {
    //  Limpa o vetor `m_staticObjects`
    m_staticObjects.clear();
    
    //  ======================================================================
    //  ROCHAS (ROCK_{1-5}, HUGE_ROCK)
    //  ======================================================================
    //  Posições das rochas (x, z)
    const std::vector<glm::vec2> rock_positions = {
        { 0.0f, -145.0f },
        { 5.0f, -145.0f },
        { 6.0f, -146.0f },
        { 10.0f, -140.0f },
        { 20.0f, -135.0f },
    };

    //  Posições das huge rocks (x, z)
    const std::vector<glm::vec2> huge_rocks_positions = {
        { 10.0f, -140.0f },
        { 15.0f, -135.0f },
        { 20.0f, -130.0f },
    };

    //  Construção das rochas
    for (int i = 0; i < rock_positions.size(); ++i) {
        //  vetor de objetos estáticos
        m_staticObjects.push_back(
            //  ponteiro para objeto estático
            std::make_shared<StaticObject>(
                //  Modelo a ser utilizado
                Assets::ROCK_3,

                //  Posição do objeto
                glm::vec3(
                    rock_positions[i].x,
                    m_terrain->getHeight(rock_positions[i].x, rock_positions[i].y) - 0.5f,
                    rock_positions[i].y
                ),

                //  Rotação do objeto
                glm::vec3(0.0f),

                //  Escala
                glm::vec3(0.05f)
            )
        );

        std::cout
            << "rock at "
            << rock_positions[i].x
            << ", "
            << m_terrain->getHeight(rock_positions[i].x, rock_positions[i].y)
            << ", "
            << rock_positions[i].y
            << '\n';
    }

    //  Construção das huge rocks
    // for (int i = 0; i < huge_rocks_positions.size(); ++i) {
    //     //  vetor de objetos estáticos
    //     m_staticObjects.push_back(
    //         //  ponteiro para objeto estático
    //         std::make_shared<StaticObject>(
    //             //  Modelo a ser utilizado
    //             Assets::HUGE_ROCK,

    //             //  Posição do objeto
    //             glm::vec3(
    //                 huge_rocks_positions[i].x,
    //                 m_terrain->getHeight(huge_rocks_positions[i].x, huge_rocks_positions[i].y) - 0.5f,
    //                 huge_rocks_positions[i].y
    //             ),

    //             //  Rotação do objeto
    //             glm::vec3(0.0f),

    //             //  Escala
    //             glm::vec3(1.0f)
    //         )
    //     );
    // };
};



/**
 * @brief   Constrói a mailbox, objetivo
 */
void Scene::buildMailbox() {
    float x = 155.0f, z = 135.0f;
    m_mailbox = std::make_shared<Mailbox>(
        glm::vec3(x, m_terrain->getHeight(x, z), z),
        glm::vec3(0.0f),
        glm::vec3(3.0f)
    );
}

/**
 * @brief   Constrói os anéis de objetivo do jogo
 */
void Scene::buildRings() {
    // Posições (x, offset_y, z) — offset_y é somado à altura do terreno
    const std::vector<glm::vec3> positions = {
        {-140.0f, 30.0f, -150.0f},
        { -90.0f, 40.0f, -100.0f},
        { -40.0f, 50.0f,  -50.0f},
        {  10.0f, 50.0f,    0.0f},
        {  60.0f, 40.0f,   50.0f},
        { 110.0f, 30.0f,  100.0f},
        { 150.0f, 20.0f,  150.0f},
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
void Scene::buildLetter()
{
    float x = -180.0f, z = -200.0f;
    m_letter =
        std::make_shared<Letter>(
            glm::vec3(
                x,
                m_terrain->getHeight(x, z) + 1.0f,
                z
            )
        );

    m_letterState =
        LetterState::OnGround;

    m_letterVelocity =
        glm::vec3(0.0f);
}


void Scene::buildButterflyNPCs()
{
    BezierPath path;

    path.addPoint(glm::vec3(-30.0f, 15.0f, -30.0f));
    path.addPoint(glm::vec3(-10.0f,25.0f, 20.0f));
    path.addPoint(glm::vec3(20.0f,25.0f, 20.0f));
    path.addPoint(glm::vec3(30.0f,15.0f,-30.0f));

    auto butterfly =
        std::make_shared<ButterflyNPC>(
            glm::vec3(0.0f),
            glm::vec3(0.0f),
            glm::vec3(2.0f)
        );

    butterfly->setPath(
        std::make_shared<BezierPath>(path)
    );

    m_butterflyNPCs.push_back(
        butterfly
    );
}

void Scene::buildCarpNPCs()
{
//     BezierPath path;

//     path.addPoint(glm::vec3(-20,1,-15));
//     path.addPoint(glm::vec3(-10,3, 20));
//     path.addPoint(glm::vec3( 10,3, 20));
//     path.addPoint(glm::vec3( 20,1,-15));
//     path.addPoint(glm::vec3( 10,2,-25));
//     path.addPoint(glm::vec3(-10,2,-25));
//     path.addPoint(glm::vec3(-20,1,-15));

//     for(int i = 0; i < 5; i++)
//     {
//         auto carp =
//             std::make_shared<CarpNPC>(
//                 glm::vec3(
//                     i * 2.0f,
//                     0.0f,
//                     i * 1.5f
//                 ),
//                 glm::vec3(0.0f),
//                 glm::vec3(1.5f)
//             );

//         carp->setPath(
//             std::make_shared<BezierPath>(
//                 path
//             )
//         );

//         m_carpNPCs.push_back(
//             carp
//         );
//     }
}