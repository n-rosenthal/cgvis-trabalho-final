/**
 * @file    `Scene.cpp`
 * @brief   Implementação da classe `Scene`
 */

#include <glad/glad.h>
#include "Game/Scene.hpp"
#include "Game/Renderer.hpp"
#include "audio/AudioManager.hpp"
#include "Collision/collisions.hpp"

#include <tuple>
#include <algorithm>
#include <random>
#include <chrono>



#include "Game/Window.hpp" // varr. depuração LETTER

extern SoundManager g_Sound;

// distancePointSegment — mantida aqui, fora do main
static float distancePointSegment(glm::vec3 p, glm::vec3 a, glm::vec3 b) {
    glm::vec3 ab = b - a;
    float t = glm::dot(p - a, ab) / glm::dot(ab, ab);
    t = glm::clamp(t, 0.0f, 1.0f);
    return length(p - (a + t * ab));
}


/**
 * @brief   Cria os bursts persistentes usados pelos trails de
 *          partícula (pássaro em voo, carta arremessada) e os
 *          insere em m_particleBursts, para que sejam desenhados
 *          e atualizados junto com os demais efeitos de partícula
 *          já existentes (anéis, entrega da carta). Guarda
 *          ponteiros observadores (não donos) para alimentá-los
 *          aos poucos em Scene::update().
 */
void Scene::buildTrails() {
    auto birdBurst = std::make_unique<ParticleBurst>();
    birdBurst->setPersistent(true);
    m_birdTrailBurst = birdBurst.get();
    m_particleBursts.push_back(std::move(birdBurst));

    auto letterBurst = std::make_unique<ParticleBurst>();
    letterBurst->setPersistent(true);
    m_letterTrailBurst = letterBurst.get();
    m_particleBursts.push_back(std::move(letterBurst));
}

/**
 * @brief   Instancia o pássaro controlável e posiciona-o no ponto de partida.
 *          Depende de m_terrain já estar construído (buildTerrain()).
 */
void Scene::spawnBird() {
    m_bird.emplace();
    m_bird->setPosition(glm::vec3(-345.0f, m_terrain->getHeight(-345.0f, -5.0f) + 0.5f, -5.0f));
}

void Scene::computeParabola() {
    if (!m_bird || !m_letter) return;

    // Ponto inicial: posição atual da carta (junto ao pássaro)
    glm::vec3 start = m_letter->getPosition();

    // Direção para a frente do pássaro (no plano horizontal)
    glm::vec3 forward = m_bird->getForward();
    forward.y = 0.0f;
    if (length(forward) < 0.001f) forward = glm::vec3(0.0f, 0.0f, -1.0f);
    forward = normalize(forward);

    // Distância de lançamento (ajustável)
    float throwDistance = 12.0f;

    // Ponto final: projeta para frente e ajusta à altura do terreno
    glm::vec3 end = start + forward * throwDistance;
    end.y = m_terrain->getHeight(end.x, end.z);

    // Ponto de controle: pico do arco (acima do ponto médio)
    glm::vec3 mid = (start + end) * 0.5f;
    float arcHeight = 5.0f;   // altura máxima do arco
    glm::vec3 control = mid + glm::vec3(0.0f, arcHeight, 0.0f);

    m_parabolaStart = start;
    m_parabolaControl = control;
    m_parabolaEnd = end;

    // Gera pontos para visualização (ex: 30 pontos)
    m_parabolaPoints.clear();
    const int segments = 30;
    for (int i = 0; i <= segments; ++i) {
        float t = (float)i / segments;
        // Curva de Bézier quadrática: B(t) = (1-t)^2*P0 + 2*(1-t)*t*P1 + t^2*P2
        glm::vec3 p = (1-t)*(1-t)*start + 2*(1-t)*t*control + t*t*end;
        m_parabolaPoints.push_back(p);
    }
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
    m_collisionSoundCooldown -= dt;

    if (m_collisionSoundCooldown < 0.0f)
        m_collisionSoundCooldown = 0.0f;

    m_bird->update(dt,
                    w, 
                    m_terrain->getHeight(m_bird->getPosition().x, m_bird->getPosition().z),
                    m_terrain->getNormal(m_bird->getPosition().x, m_bird->getPosition().z));
    
    //  depuração de LETTER
    g_DebugLetterPosition = m_letter->getPosition();
    g_DebugLetterRotation = m_letter->getRotation();
    
       // --------------------------------------------------
    // Sistema de parábola da carta (tecla G)
    // --------------------------------------------------
    static bool gHeld = false;
    bool gPressed = glfwGetKey(w, GLFW_KEY_G) == GLFW_PRESS;

    if (m_letterState == LetterState::Carried) {
        if (gPressed && !gHeld) {
            // --- Início do pressionamento: calcular parábola ---
            m_parabolaActive = true;
            computeParabola();
            m_throwProgress = 0.0f;
        }

        if (gPressed) {
            computeParabola();
        }

        if (!gPressed && gHeld) {
            // --- Soltar G: iniciar o lançamento ---
            if (m_parabolaActive) {
                
            g_Sound.play(
                "assets/audio/dragon-studio-hawk-call-sound-effect-hawk-cry-364472.mp3"
            );

                m_letterState = LetterState::Thrown;
                m_parabolaActive = false;
                m_throwProgress = 0.0f;
                // Câmera segue a carta
                m_cameraMode = CameraMode::FollowLetter;
            }
        }
    } else {
        // Se a carta não estiver carregada, não faz nada
        m_parabolaActive = false;
        m_parabolaPoints.clear();
    }

    gHeld = gPressed;

    // --------------------------------------------------
    // Atualiza a carta (inclui novo estado Thrown)
    // --------------------------------------------------
    updateLetter(dt, w);

    // --------------------------------------------------
    // Camera update
    // --------------------------------------------------
    switch (m_cameraMode)
    {
        case CameraMode::FollowBird:
            m_camera.update(
                m_bird->getPositionNoBob(),
                m_bird->getForwardNoBob(),
                m_bird->getUpNoBob(),
                dt
            );
            break;

        case CameraMode::FollowLetter:
        {
            // Orbita acima e ao lado da carta, olhando para ela
            glm::vec3 lpos = m_letter->getPosition();
            m_camera.lookAt(lpos, glm::vec3(0.0f, 8.0f, 12.0f));
            // Se a carta pousar, volta para o pássaro
            if (m_letterState == LetterState::OnGround ||
                m_letterState == LetterState::Falling)
            {
                m_cameraMode = CameraMode::FollowBird;
            }
            break;
        }

        case CameraMode::WinScreen:
        {
            // Câmera olha para a mailbox de cima, ligeiramente afastada
            glm::vec3 mpos = m_mailbox->getPosition();
            m_camera.lookAt(mpos, glm::vec3(0.0f, 10.0f, 15.0f));
            break;
        }
    }


    // --------------------------------------------------
    // Trails de partícula
    // --------------------------------------------------
    m_birdTrailEmitter.update(dt);
    m_letterTrailEmitter.update(dt);

    if (!m_bird->getStanding() && m_birdTrailBurst)
    {
        m_birdTrailEmitter.emitAt(
            *m_birdTrailBurst,
            m_bird->getPositionNoBob(),
            1.0f
        );
    }

    if (m_letterState == LetterState::Thrown && m_letterTrailBurst)
    {
        m_letterTrailEmitter.emitAt(
            *m_letterTrailBurst,
            m_letter->getPosition(),
            1.5f
        );
    }

    for(auto& ring : m_rings)
    {
        if(ring)
        {
            ring->update(dt);
        }
    }

    m_rings.erase(
        std::remove_if(
            m_rings.begin(),
            m_rings.end(),
            [](const std::shared_ptr<Ring>& ring)
            {
                return !ring || ring->isDead();
            }
        ),
        m_rings.end()
    );

    if (m_mailbox)
        m_mailbox->update(dt);


    // NPCs

    for(auto& npc : m_butterflyNPCs) {
        npc->update(dt);
    }


    for (auto& npc : m_duckNPCs) {
        npc->update(dt);
    }
    
    for(auto& burst : m_particleBursts)
    {
        burst->update(dt);
    }

    m_particleBursts.erase(
        std::remove_if(
            m_particleBursts.begin(),
            m_particleBursts.end(),
            [](const auto& b)
            {
                return b->finished();
            }
        ),
        m_particleBursts.end()
    );
}

void Scene::updateLetter(float dt, GLFWwindow* window) {
    if (!m_letter) return;

    switch (m_letterState) {
        case LetterState::Carried:
        {
            glm::vec3 offset =
                m_bird->getForward() * 2.0f
                -
                m_bird->getUp() * 0.5f;

            m_letter->setPosition(
                m_bird->getPosition() + offset
            );

            m_letter->setRotation(
                m_bird->getRotation()
            );

            break;
        }

        case LetterState::Falling:
        {
            m_letterVelocity.y -= 9.8f * dt;
            glm::vec3 p = m_letter->getPosition();
            p += m_letterVelocity * dt;
            m_letter->setPosition(p);
            break;
        }

        case LetterState::Thrown:
        {
            m_throwProgress +=
                m_throwSpeed * dt;

            if (m_throwProgress >= 1.0f)
            {
                m_throwProgress = 1.0f;

                glm::vec3 finalPos =
                    m_parabolaEnd;

                m_letter->setPosition(
                    finalPos
                );

                m_letterVelocity =
                    glm::vec3(0.0f);

                // deixa o sistema de colisão
                // decidir quando ela toca o chão
                m_letterState =
                    LetterState::Falling;

                break;
            }

            float t =
                m_throwProgress;

            glm::vec3 p =
                (1-t)*(1-t)*m_parabolaStart
                +
                2*(1-t)*t*m_parabolaControl
                +
                t*t*m_parabolaEnd;

            m_letter->setPosition(p);

            break;
        }

        case LetterState::OnGround:
        {
            //  animação de oscilação vertical de `Letter`
            //  tempo de animação
            m_letterOscillationTime += dt;

            // varr. `Letter`
            glm::vec2 ground = m_letter->getGroundPos();
            float h = m_letter->getFloatHeight();
            float d = m_letter->getAmplitude();
            float phase = m_letter->getPhase();

            // offset vertical a partir de onda senoidal
            float omega = 1.5f;              // freq. angular ômega (rad/s)
            float offset = d * sin(omega * m_letterOscillationTime + phase);

            //  obtém altura do terreno em (x, z)
            float terrainY = m_terrain->getHeight(ground.x, ground.y);

            //  nova posição
            float y = terrainY + h + offset;
            m_letter->setPosition(glm::vec3(ground.x, y, ground.y));

            // rotação contínua sobre Y
            glm::vec3 rot = m_letter->getRotation();
            float rotSpeed = 0.8f;          // radians per second
            rot.y += rotSpeed * dt;
            
            //  intervalo de rotação
            if (rot.y > 2.0f * M_PI) rot.y -= 2.0f * M_PI;
            m_letter->setRotation(rot);

            break;
        }
    };
};



/**
 * @brief   Resolve colisões da cena
 */
void Scene::resolveCollisions() {
    // ============================================================
    // Dados do Bird
    // ============================================================

    const glm::vec3 birdPos = m_bird->getPosition();
    const glm::vec3 birdVel = m_bird->getVelocity();

    const CapsuleCollider& birdCollider =
        m_bird->getCollider();

    // ============================================================
    // 1. Colisão Bird ↔ Terreno
    // ============================================================

    const float terrainHeight =
        m_terrain->getHeight(
            birdPos.x,
            birdPos.z
        );

    if(birdPos.y - 0.6f <= terrainHeight)
    {
        glm::vec3 normal =
            m_terrain->getNormal(
                birdPos.x,
                birdPos.z
            );

        if(m_bird->onTerrainCollision(
            terrainHeight,
            normal
        ))
        {

            glm::vec3 pos =
                birdPos - normal * 0.5f;

            float speed =
                length(birdVel);

            spawnBurst(
                pos,
                speed * speed,
                speed,
                speed * speed * 0.2f
            );
        }
    }

    // ============================================================
    // 2. Colisão Bird ↔ Objetos Estáticos
    // ============================================================

    for (auto& obj : m_staticObjects)
    {
        if (!obj)
            continue;

        switch (obj->getType())
        {
            // ====================================================
            // ÁRVORES
            // ====================================================

            case StaticObjectType::OAK:
            {
                const auto& colliders = obj->getColliders();

                for (const auto& collider : colliders)
                {
                    if (!CollisionSystem::intersects(
                            birdCollider,
                            *collider))
                    {
                        continue;
                    }

                    switch (collider->tag)
                    {
                        // ----------------------------------------
                        // Copa
                        // ----------------------------------------

                        case ColliderTag::TreeCanopy: {
                            float speed =
                                glm::length(birdVel);

                            if (speed > 0.01f) { m_bird->setVelocity(birdVel * 0.5f); }
                            if (m_collisionSoundCooldown <= 0.0f) {
                                g_Sound.play(
                                    "assets/audio/tanweraman-leaves-rustling-2-329002.mp3"
                                );
                                m_collisionSoundCooldown = 1.21f;
                            }
                            
                            spawnBurst(
                                birdPos,
                                speed * speed,
                                speed,
                                speed * speed * 0.2f,
                                Colors::TREE
                            );
                            
                            break;
                        }

                        // ----------------------------------------
                        // Tronco
                        // ----------------------------------------

                        case ColliderTag::TreeTrunk:
                        {
                            glm::vec3 normal =
                                birdPos - obj->getPosition();

                            normal.y = 0.0f;

                            if (glm::length(normal) > 0.001f)
                                normal =
                                    glm::normalize(normal);

                            glm::vec3 impulse =
                                normal * 20.0f
                                + glm::vec3(0.0f, 10.0f, 0.0f);

                            m_bird->setVelocity(
                                impulse
                            );

                            if (m_collisionSoundCooldown <= 0.0f) {
                                g_Sound.play(
                                    "assets/audio/cartoonish-stone-sfx-slow.wav"
                                );
                                m_collisionSoundCooldown = 1.21f;
                            }

                            float speed =
                                glm::length(birdVel);

                            spawnBurst(
                                birdPos,
                                speed * speed,
                                speed,
                                speed * speed * 0.2f,
                                Colors::TRUNK
                            );

                            break;
                        }

                        default:
                            break;
                    }
                }

                break;
            }

            // ====================================================
            // ROCHAS
            // ====================================================

            case StaticObjectType::ROCK_8:
            case StaticObjectType::ROCK_9:
            case StaticObjectType::ROCK_7:
            {
                if (CollisionSystem::collidablesIntersect(
                        *m_bird,
                        *obj))
                {
                    glm::vec3 normal =
                        birdPos - obj->getPosition();

                    normal.y = 0.0f;

                    if (glm::length(normal) > 0.001f)
                        normal =
                            glm::normalize(normal);

                    glm::vec3 impulse =
                        normal * 20.0f
                        + glm::vec3(0.0f, 10.0f, 0.0f);

                    m_bird->setVelocity(
                        impulse
                    );

                    if (m_collisionSoundCooldown <= 0.0f) {
                        g_Sound.play(
                            "assets/audio/cartoonish-stone-sfx-slow.wav"
                        );
                        m_collisionSoundCooldown = 1.2f;
                    }

                    //  ----------------------------------------
                    int t;
                    if (obj->getType() == StaticObjectType::ROCK_7)         t = 7;
                    else if (obj->getType() == StaticObjectType::ROCK_8)    t = 8;
                    else                                                    t = 9;

                    float speed =
                        glm::length(birdVel);

                    spawnBurst(
                        birdPos,
                        speed * speed,
                        speed,
                        speed * speed * 0.2f,
                        Colors::ROCK
                    );
                }

                break;
            }

            // ====================================================
            // CASA
            // ====================================================

            case StaticObjectType::HOUSE:
            {
                if (CollisionSystem::collidablesIntersect(
                        *m_bird,
                        *obj))
                {
                    glm::vec3 normal =
                        birdPos - obj->getPosition();

                    normal.y = 0.0f;

                    if (glm::length(normal) > 0.001f)
                        normal =
                            glm::normalize(normal);

                    glm::vec3 impulse =
                        normal * 20.0f
                        + glm::vec3(0.0f, 10.0f, 0.0f);

                    m_bird->setVelocity(
                        impulse
                    );

                    printf("casa::colisão em (x, y, z) = (%f, %f, %f)\n",
                        normal.x, normal.y, normal.z);

                    if (m_collisionSoundCooldown <= 0.0f) {
                        g_Sound.play(
                            "assets/audio/cartoonish-stone-sfx-slow.wav"
                        );
                        m_collisionSoundCooldown = 1.2f;
                    }

                    float speed =
                        glm::length(birdVel);

                    spawnBurst(
                        birdPos,
                        speed * speed,
                        speed,
                        speed * speed * 0.2f,
                        Colors::ROCK
                    );
                }

                break;
            }

            // ====================================================
            // MAILBOX
            // ====================================================

            case StaticObjectType::MAILBOX:
            {
                // Mailbox já possui lógica própria
                break;
            }

            // ====================================================
            // ARBUSTOS E PLANTAS
            // ====================================================

            case StaticObjectType::BUSH:
            case StaticObjectType::SHRUB:
            case StaticObjectType::MARINE_PLANT:
            {
                if (CollisionSystem::collidablesIntersect(
                        *m_bird,
                        *obj))
                {
                    float speed =
                        glm::length(birdVel);

                    m_bird->setVelocity(
                        birdVel * 0.8f
                    );

                    if (m_collisionSoundCooldown <= 0.0f) {
                        g_Sound.play(
                            "assets/audio/crushing-leafs-cinematic-fx_E_minor.wav"
                        );
                        m_collisionSoundCooldown = 1.2f;
                    }

                    spawnBurst(
                        birdPos,
                        speed * speed,
                        speed,
                        speed * speed * 0.1f,
                        Colors::TREE
                    );
                }

                break;
            }

            default:
                break;
        }
    }

    // ============================================================
    // 3. Colisão Bird ↔ Letter (captura)
    // ============================================================

    if (
        m_letter &&
        (
            m_letterState == LetterState::OnGround ||
            m_letterState == LetterState::Falling
        )
    )
    {
        if (
            CollisionSystem::collidablesIntersect(
                *m_bird,
                *m_letter
            )
        )
        {
            printf("[Letter] Capturada\n");

            m_letterState =
                LetterState::Carried;

            m_letterVelocity =
                glm::vec3(0.0f);

            g_Sound.play(
                "assets/audio/alexzavesa-cartoon-4-468375.mp3"
            );
        }
    }

    // ============================================================
    // 4. Colisão Letter ↔ Terreno
    // ============================================================

    if (
        m_letter &&
        (
            m_letterState == LetterState::Falling ||
            m_letterState == LetterState::Thrown
        )
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

        if (
            p.y <= terrainHeight + letterRadius
        )
        {
            p.y =
                terrainHeight +
                letterRadius;

            m_letter->setPosition(p);

            // IMPORTANTE
            m_letter->setGroundPos(
                glm::vec2(
                    p.x,
                    p.z
                )
            );

            m_letterVelocity =
                glm::vec3(0.0f);

            m_letterState =
                LetterState::OnGround;

            m_letterOscillationTime =
                0.0f;

            printf(
                "[Letter] Colisão com terreno em (%f,%f)\n",
                p.x,
                p.z
            );

            g_Sound.play(
                "assets/audio/cartoon-boing-bouncy-big_F_major.wav"
            );
        }
    }

    // ============================================================
    // 5. Colisão Letter ↔ Mailbox
    // ============================================================

    if(
        m_letter &&
        m_mailbox &&
        m_letterState == LetterState::Thrown
    )
    {
        if(
            CollisionSystem::collidablesIntersect(
                *m_letter,
                *m_mailbox
            )
        )
        {
            printf("Colisão de carta com mailbox!\n");

            m_letterState = LetterState::OnGround;

            glm::vec3 mailboxPos =
                m_mailbox->getPosition();

            m_letter->setGroundPos(
                glm::vec2(
                    mailboxPos.x,
                    mailboxPos.z
                )
            );

            m_letter->setFloatHeight(9.5f);

            m_letterVelocity =
                glm::vec3(0.0f);

            m_throwProgress =
                0.0f;

            m_mailbox->activate();

            // Câmera foca na mailbox — tela de vitória
            m_cameraMode = CameraMode::WinScreen;

            g_Sound.play(
                "assets/audio/cartoon-music-game-sfx-cartoon-game-upgrade-494470.mp3"
            );

            // Central burst at mailbox
            spawnBurst(
                m_mailbox->getPosition() + glm::vec3(0.0f, 2.0f, 0.0f),
                80,
                5.0f,
                1.2f,
                Colors::MAIL
            );
            // Additional decorative bursts around mailbox using existing palette
            glm::vec3 mbPos = m_mailbox->getPosition();
            spawnBurst(mbPos + glm::vec3(0.0f, 2.5f, 0.0f), 40, 4.0f, 1.0f, Colors::RING);
            spawnBurst(mbPos + glm::vec3(2.0f, 2.0f, 0.0f), 30, 3.5f, 0.9f, Colors::TREE);
            spawnBurst(mbPos + glm::vec3(-2.0f, 2.0f, 0.0f), 30, 3.5f, 0.9f, Colors::TRUNK);

            printf(
                "Carta entregue! Jogo vencido.\n"
            );
        }
    }

    // ============================================================
    // 6. Colisão Bird ↔ Anéis
    // ============================================================

    for (auto& ring : m_rings)
    {
        if (!ring)
            continue;

        if (
            !ring->collected() &&
            CollisionSystem::collidablesIntersect(
                *m_bird,
                *ring
            )
        )
        {
            ring->collect();

            glm::vec3 c =
                ring->getPosition();

            float r =
                ring->getRadius();

            std::vector<glm::vec3> points =
            {
                c + glm::vec3(0.0f,  r, 0.0f),
                c + glm::vec3(0.0f, -r, 0.0f),
                c + glm::vec3( r, 0.0f, 0.0f),
                c + glm::vec3(-r, 0.0f, 0.0f),
                c + glm::vec3(0.0f, 0.0f,  r),
                c + glm::vec3(0.0f, 0.0f, -r)
            };

            constexpr float lifetime = 10.0f;
            constexpr int   count    = 120;
            constexpr float speed    = 1.0f;

            g_Sound.play(
                "assets/audio/drum-roll-and-bell_112bpm.wav"
            );

            for (const auto& p : points)
            {
                spawnBurst(
                    p,
                    count,
                    speed,
                    lifetime,
                    Colors::RING
                );
            }
        }
    }

    m_rings.erase(
        std::remove_if(
            m_rings.begin(),
            m_rings.end(),
            [](const std::shared_ptr<Ring>& ring)
            {
                return !ring || ring->isDead();
            }
        ),
        m_rings.end()
    );

    // ============================================================
    // 7. Colisão Letter ↔ Objetos Estáticos
    // ============================================================

    if(
        m_letter &&
        (
            m_letterState == LetterState::Falling ||
            m_letterState == LetterState::Thrown
        )
    )
    {
        for(auto& obj : m_staticObjects)
        {
            if(
                CollisionSystem::collidablesIntersect(
                    *m_letter,
                    *obj
                )
            )
            {
                printf("Colisão de `Letter` contra `StaticObject`\n");

                m_letterState =
                    LetterState::OnGround;

                glm::vec3 p =
                    m_letter->getPosition();

                p.y =
                    obj->getPosition().y +
                    0.5f;

                m_letter->setPosition(p);

                m_letterVelocity =
                    glm::vec3(0.0f);

                g_Sound.play(
                    "assets/audio/cartoon-boing-bouncy-big_F_major.wav"
                );

                break;
            }
        }
    }
}



/**
 * @brief   Invoca o renderizador para desenhar os objetos propriamente
 * @param   r   (Renderer&)
 *              renderizador
 *  
 */
void Scene::draw(Renderer& r) {
    if (m_bird->getStanding()) {
        r.drawBird(*m_bird, true);
    } else {
        r.drawBird(*m_bird, false);
    }

    r.drawTerrain(*m_terrain);
    r.drawRings(m_rings);

    if (m_letter) r.drawLetter(*m_letter);
    r.drawMailbox(*m_mailbox);
    r.drawObjects(m_staticObjects);

    // NPCs
    for (auto& npc : m_butterflyNPCs) {
        r.drawButterflyNPC(*npc);
    }

    for (auto& npc : m_duckNPCs) {
        r.drawDuckNPC(*npc);
}

    r.drawMailbox(*m_mailbox);

    // --- Desenha a parábola da carta ---
    r.drawParabola(m_parabolaPoints, m_parabolaActive);

    // Partículas
    GLint currentProgram;

    glGetIntegerv(
        GL_CURRENT_PROGRAM,
        &currentProgram
    );

    r.drawParticles(
        m_particleBursts
    );
    
}



void Scene::spawnBurst(
    const glm::vec3& position,
    int count,
    float speed,
    float lifetime,
    const glm::vec4& color
)
{
    m_particleBursts.push_back(
        std::make_unique<ParticleBurst>(
            position,
            count,
            speed,
            lifetime,
            color
        )
    );
}

//  ======================================================================
//  Métodos `build*()` para construção dos objetos da cena virtual
//  ======================================================================
/**
 * @brief   Constrói o terreno do jogo
 */
void Scene::buildTerrain() {
    m_terrain = std::make_unique<Terrain>(
        190.0f,
        190.0f,
        4.5f
    );

    assert(m_terrain);
    m_terrain->generate();
}

float Scene::terrainSlope(float x, float z) const {
    constexpr float eps = 1.0f;

    float h1 = m_terrain->getHeight(x + eps, z);
    float h2 = m_terrain->getHeight(x - eps, z);

    float h3 = m_terrain->getHeight(x, z + eps);
    float h4 = m_terrain->getHeight(x, z - eps);

    float dx = (h1 - h2) / (2.0f * eps);
    float dz = (h3 - h4) / (2.0f * eps);

    return std::sqrt(dx*dx + dz*dz);
}

float Scene::lakeDistance(float x, float z) const {
    float ex = x / m_terrain->lakeRX();
    float ez = z / m_terrain->lakeRZ();

    return std::sqrt(ex*ex + ez*ez);
}


/**
 * @brief   Constrói, em posições, rotação e escala aleatórias,
 *          os objetos estáticos interiores ao lago, na cena virtual
 * 
 * @details Atualmente, existe somente
 *              Assets::MARINE_PLANT
 * 
 *          enquanto `lakeObject`
 * 
 * @param   count   (int)
 *                  quantidade de objetos a serem construídos
 * 
 * @return  std::vector<StaticObjectDef> 
 *                  lista de objetos construídos
 */
std::vector<StaticObjectDef> Scene::generateLakeObjects(int count) {
    //  inicializa vetor de resposta
    std::vector<StaticObjectDef> out;

    //  cria gerador aleatório
    std::mt19937 rng(42);

    //  cria distribuições aleatórias
    std::uniform_real_distribution<float>
        px(-300.f,300.f),
        pz(-300.f,300.f),
        rot(0.f,360.f),
        scale(0.2f,0.8f);

    //  considerando cada modelo (= 1, atualmente) de `lakeObjects`,...
    const ModelDefinition* lakeModels[] = {
        &Assets::MARINE_PLANT
    };

    //  para [0, size - 1], faça...
    while (out.size() < count) {
        //  gera posições aleatórias
        float x = px(rng);
        float z = pz(rng);
        float y = m_terrain->getHeight(x,z);

        //  gera rotação aleatória
        float r = rot(rng);
        glm::vec3 rot = glm::vec3(0.0f, r, 0.0f);

        //  escala
        float s = scale(rng);

        //  distância ao centro do lago determina
        //  se prosseguiremos ou não com o objeto atual
        float lake = lakeDistance(x,z);
        if(lake > 1.1f || lake < 0.6f) continue;
        
        //  "escolhe" (1/1) modelo atual
        auto model = lakeModels[0];

        //  constrói estrutura e adiciona ao vetor
        out.emplace_back(
            StaticObjectDef{
                .model      = model,
                .type       = StaticObjectType::MARINE_PLANT,
                .position   = glm::vec3(x,y,z),
                .rotation   = rot,
                .scale      = glm::vec3(s,s,s),
                .colliders  = {
                    //  usa colisor esférico para `Assets::MARINE_PLANT`
                    std::make_shared<SphereCollider>(
                        glm::vec3(0.0f,2.0f,0.0f),      //  centro do colisor essférico
                        2.0f                            //  raio do colisor essférico
                    )
                }
            }
        );
    }

    return out;
}


/**
 * @brief       Constrói, em posições, rotação e escala aleatórias,
 *              os objetos estáticos do tipo `treeObjects`, na cena virtual
 * 
 * @details     Atualmente, existe somente
 *                  Assets::OAK
 * 
 *              enquanto `treeObjects`
 *
 * 
 * @param       count   (int)
 *                      quantidade de objetos a serem construídos
 * @return      std::vector<StaticObjectDef> 
 *                      lista de objetos construídos
 */
std::vector<StaticObjectDef> Scene::generateTrees(int count) {
    //  inicializa vetor de resposta
    std::vector<StaticObjectDef> out;

    //  cria gerador aleatório
    std::mt19937 rng(42);

    //  cria distribuições aleatórias
    std::uniform_real_distribution<float>
        px(-300.f,300.f),
        pz(-300.f,300.f),
        rot(0.f,360.f),
        scale(2.5f,6.0f);

    //  considerando cada modelo (= 1, atualmente) de `treeObjects`,...
    const ModelDefinition* treeModels[] = {
        &Assets::OAK
    };

    //  para [0, size - 1], faça...
    while(out.size() < count) {
        //  gera posições aleatórias
        float x = px(rng);
        float z = pz(rng);
        float y = m_terrain->getHeight(x,z);

        //  gera rotação aleatória
        float r = rot(rng);
        glm::vec3 rot = glm::vec3(0.0f, r, 0.0f);

        //  escala
        float s = scale(rng);

        //  definir os colisores da árvore
        //  colisor da copa
        auto canopy =
            //  a copa recebe um colisor esférico
            std::make_shared<SphereCollider>(
                //  centro: (h(x,z) * 2))
                glm::vec3(0.0f, s*3.5f, 0.0f),

                //  raio:
                0.45f * s
            );
        
        //  tag: define a consequência da colisão
        //  TreeCanopy: redução da velocidade, colisão "falsa"
        canopy->tag = ColliderTag::TreeCanopy;

        //  colisor do tronco
        auto trunk =
            //  o tronco recebe um colisor cilíndrico
            std::make_shared<CylindricalCollider>(
                //  centro: (h(x,z) * 0.5f))
                glm::vec3(0.0f, 0.0f, 0.0f),

                //  raio: (s * 0.12f)
                s*0.2f,

                //  altura: s
            7.0f * s
            );

        //  tag: define a consequência da colisão
        //  TreeTrunk: redução da velocidade, colisão verdadeira
        trunk->tag = ColliderTag::TreeTrunk;


        //  utiliza distância ao lago e inclinação das montanhas
        //  para decidir se prosseguiremos ou não com o objeto atual
        float slope = terrainSlope(x,z);
        float lake  = lakeDistance(x,z);
        if (
            slope > 0.6f ||
            lake < 1.2f ||
            lake > 2.3f
        )   continue;

        //  "escolhe" (1/1) modelo atual
        auto model = treeModels[0];

        out.emplace_back(
            StaticObjectDef{
                .model      = model,
                .type       = StaticObjectType::OAK,
                .position   = glm::vec3(x,y,z),
                .rotation   = rot,
                .scale      = glm::vec3(s, s, s),
                .colliders  = {
                    canopy,
                    trunk
                }
            }
        );
    }

    return out;
}


/**
 * @brief   Constrói, em posições, rotação e escala aleatórias,
 *          os objetos estático de arbustos, na cena virtual
 * 
 * @details Atualmente, existem somente
 *              Assets::STYLED_SHRUB
 *              Assets::SHRUB
 *              Assets::BUSH
 * 
 *          enquanto `busheObject`s
 * 
 * @param   count (int)
 *                  quantidade de objetos a serem construídos
 * @return  std::vector<StaticObjectDef> 
 *                  lista de objetos construídos
 */
std::vector<StaticObjectDef> Scene::generateBushes(int count) {
    //  inicializa vetor de resposta
    std::vector<StaticObjectDef> out;

    //  cria gerador aleatório
    std::mt19937 rng(43);

    //  cria distribuições aleatórias
    std::uniform_real_distribution<float>
        px(-300.f,300.f),
        pz(-300.f,300.f),
        rot(0.f,360.f),
        scale(4.0f, 7.0f);


    //  modelos possíveis
    const ModelDefinition* bushObjects[] =
    {
        &Assets::STYLED_SHRUB,
        // &Assets::SHRUB
    };
    

    //  considerando cada modelo de `bushObjects`, em [0..size-1]
    while(out.size() < count) {
        //  gera posições aleatórias
        float x = px(rng);
        float z = pz(rng);
        float y = m_terrain->getHeight(x,z);

        //  rotação aleatória
        float r = rot(rng);

        //  escala aleatória
        float s = scale(rng);

        //  utiliza distância ao lago e inclinação das montanhas
        //  para decidir se prosseguiremos ou não com o objeto atual
        float lake = lakeDistance(x,z);
        float slope = terrainSlope(x,z);

        //  colisor para arbustos
        //  sempre colisor esférico com tag `ColliderTag::Shrub`
        //      colisão "falsa" (redução da velocidade)
        auto collider = std::make_shared<SphereCollider>(
            glm::vec3(0.0f, s/2.0f, 0.0f),
            s/2.4f
        );
        collider->tag = ColliderTag::Shrub;

        if (
            slope > 0.5f    ||
            lake < 1.1f     ||
            lake > 1.9f
        )   continue;

        //  seleciona o modelo
        auto model = bushObjects[rng()%1];

        //  adiciona ao vetor de resposta
        out.emplace_back(
            StaticObjectDef{
                .model      = model,
                .type       = StaticObjectType::BUSH,
                .position   = glm::vec3(x,y,z),
                .rotation   = glm::vec3(0.0f,r,0.0f),
                .scale      = glm::vec3(s, s, s),
                .colliders  = { collider }
            }
        );
    }

    return out;
}


/**
 * @brief   Constrói, em posições, rotação e escala aleatórias,
 *          os objetos estático de rochas, na cena virtual
 * 
 * @details Atualmente, existem somente
 *              Assets::ROCK_2
 *              Assets::ROCK_4
 *              Assets::ROCK_5
 * 
 *          enquanto `rockObject`s
 * 
 * @param   count   (int)
 *                  quantidade de objetos a serem construídos
 * @return  std::vector<StaticObjectDef> 
 *                  lista de objetos construídos
 */
std::vector<StaticObjectDef> Scene::generateRocks(int count) {
    std::vector<StaticObjectDef> out;
    std::mt19937 rng(44);

    std::uniform_real_distribution<float>
        px(-300.f, 300.f),
        pz(-300.f, 300.f),
        rot(0.f, 360.f),
        scale(2.0f, 5.5f);

    const ModelDefinition* rocks[] = {
        // &Assets::ROCK_6,
        // &Assets::ROCK_7,
        &Assets::ROCK_8,
        &Assets::ROCK_9
    };

    while (out.size() < count) {
        float x = px(rng);
        float z = pz(rng);
        float s = scale(rng);

        // Posiciona a rocha com um pequeno offset para cima
        float y = m_terrain->getHeight(x, z) + 0.5f;

        float r = rot(rng);
        auto model = rocks[rng() % 2];


        StaticObjectType t;
        if      (model == &Assets::ROCK_8) t = StaticObjectType::ROCK_8;
        else if (model == &Assets::ROCK_9) t = StaticObjectType::ROCK_9;

        //  ROCK_9::
        //      colisor cilíndrico (centro, r, h)
        //      com tag `ColliderTag::Rock`
        auto rock_9_collider =
            std::make_shared<SphereCollider>(
                glm::vec3(0.0f, s, 0.0f),
                2.5f * s
            );
        rock_9_collider->tag = ColliderTag::Rock;

        //  ROCK_8::
        //      colisor esférico (centro, r)
        //      com tag `ColliderTag::Rock`
        auto rock_8 =
            std::make_shared<SphereCollider>(
                glm::vec3(0.0f, s, 0.0f),
                2.5f * s
            );
        rock_8->tag = ColliderTag::Rock;

        auto collider = t == StaticObjectType::ROCK_8
            ? rock_8
            : rock_9_collider;

        float lake = lakeDistance(x, z);
        if (lake < 1.5f || lake > 2.0f) continue;

        out.emplace_back(
            StaticObjectDef{
                .model      = model,
                .type       = t,
                .position   = glm::vec3(x, m_terrain->getHeight(x, z), z) - glm::vec3(0.0f, 0.5f, 0.0f),
                .rotation   = glm::vec3(0.0f, r, 0.0f),
                .scale      = glm::vec3(s, s, s),
                .colliders  = { collider }
            }
        );
    };


    // --- Rochas no centro do lago ---
    ModelDefinition* rock_7 = &Assets::ROCK_7;

    for (int i = 0; i < 10; ++i) {
        float x = px(rng);
        float z = pz(rng);
        float r = rot(rng);
        float s = scale(rng);

        float lake = lakeDistance(x, z);
        if (lake > 0.6f) { i--; continue; }

        auto collider =
            std::make_shared<SphereCollider>(
                glm::vec3(0.0f, s, 0.0f),
                2.5f * s
            );
        collider->tag = ColliderTag::Rock;

        out.emplace_back(
            StaticObjectDef{
                .model      = rock_7,
                .type       = StaticObjectType::ROCK_7,
                .position   = glm::vec3(x, m_terrain->getHeight(x, z), z) - glm::vec3(0.0f, 0.5f, 0.0f),
                .rotation   = glm::vec3(0.0f, r, 0.0f),
                .scale      = glm::vec3(s, s, s),
                .colliders  = { collider }
            }
        );
    }
    
    // No construtor da rocha, após updateColliders():
    return out;
}

/**
 * @brief   Constrói os objetos estáticos do jogo
 */
void Scene::buildStaticObjects() {
    m_staticObjects.clear();

    std::vector<StaticObjectDef> objs;

    auto trees      = generateTrees(30);        glfwPollEvents();
    auto bushes     = generateBushes(50);       glfwPollEvents();
    auto rocks      = generateRocks(45);        glfwPollEvents();
    auto lake_objs  = generateLakeObjects(100); glfwPollEvents();

    objs.insert(objs.end(), trees.begin(), trees.end());
    objs.insert(objs.end(), bushes.begin(), bushes.end());
    objs.insert(objs.end(), rocks.begin(), rocks.end());
    objs.insert(objs.end(), lake_objs.begin(), lake_objs.end());

    for (size_t i = 0; i < objs.size(); ++i) {
        const auto& [model, type, position, rotation, scale, colliders] = objs[i];

        m_staticObjects.push_back(
            std::make_shared<StaticObject>(
                *model,
                position,
                rotation,
                scale,
                type,
                colliders
            )
        );


        if (i % 5 == 0) glfwPollEvents();
    }

    glfwPollEvents();

    // --- Casa ---
    ModelDefinition* house_model = &Assets::HOUSE;
    glm::vec3 house_position = glm::vec3(245.0f, m_terrain->getHeight(245.0f, -5.0f) - 1.5f, -5.0f);
    glm::vec3 house_rotation = glm::vec3(0.0f);
    glm::vec3 house_scale = glm::vec3(0.2f);

    auto house_collider = std::make_shared<AABBCollider>(
        glm::vec3(-8.0f, 0.0f, -8.0f),
        glm::vec3( 16.0f, 18.0f, 16.0f)
    );
    house_collider->tag = ColliderTag::House;

    m_staticObjects.push_back(
        std::make_shared<StaticObject>(
            *house_model,
            house_position,
            house_rotation,
            house_scale,
            StaticObjectType::HOUSE,
            std::vector<std::shared_ptr<Collider>>{ house_collider }
        )
    );
    




    // Transforma colliders de local -> global
    for (auto& obj : m_staticObjects) {
        obj->updateColliders();
    }
}



/**
 * @brief   Constrói a mailbox, objetivo
 */
void Scene::buildMailbox() {
    float x = 235.0f, z = -20.0f;
    m_mailbox = std::make_shared<Mailbox>(
        glm::vec3(x, m_terrain->getHeight(x, z), z),
        glm::vec3(0.0f, glm::radians(45.0f), 0.0f),   // apenas Y
        glm::vec3(2.0f)                               // escala uniforme
    );
}

/**
 * @brief   Constrói os anéis de objetivo do jogo
 */
void Scene::buildRings()
{
    const std::vector<glm::vec3> positions =
    {
        // saída da área inicial
        {-300.0f, 20.0f,  -10.0f},
        {-250.0f, 25.0f,   -5.0f},

        // aproximação do vale
        {-190.0f, 30.0f,   10.0f},
        {-140.0f, 35.0f,   35.0f},

        // borda norte do lago
        { -90.0f, 40.0f,   70.0f},
        { -30.0f, 45.0f,   95.0f},

        // atravessando região central
        {  40.0f, 50.0f,   85.0f},
        { 100.0f, 45.0f,   55.0f},

        // descendo em direção ao destino
        { 150.0f, 40.0f,   20.0f},
        { 190.0f, 35.0f,  -10.0f},

        // aproximação final
        { 220.0f, 25.0f,  -20.0f}
    };

    for (const auto& p : positions)
    {
        float groundY =
            m_terrain->getHeight(
                p.x,
                p.z
            );

        m_rings.push_back(
            std::make_shared<Ring>(
                glm::vec3(
                    p.x,
                    groundY + p.y,
                    p.z
                )
            )
        );
    }
}


/**
 *  @brief  construtor para a letter
 * m_bird->setPosition(glm::vec3(-345.0f, m_terrain->getHeight(-345.0f, -5.0f) + 0.5f, -5.0f));
 */
void Scene::buildLetter() {
    float x = -340.0f, z = -5.0f;
    float terrainY = m_terrain->getHeight(x, z);
    m_letter = std::make_shared<Letter>(glm::vec3(x, terrainY + 2.0f, z));
    m_letter->setGroundPos(glm::vec2(x, z));
    m_letter->setFloatHeight(2.0f);
    m_letter->setAmplitude(0.8f);
    m_letter->setPhase(0.0f);
    m_letterState = LetterState::OnGround;
    m_letterVelocity = glm::vec3(0.0f);
    m_letterOscillationTime = 0.0f;
}

void Scene::buildButterflyNPCs() {
    // Cria gerador aleatório
    std::mt19937 rng(42);
    
    // Distribuição para variação
    std::uniform_real_distribution<float> e(-5.5f, 5.5f);
    
    // Número de borboletas
    const int numButterflies = 4;
    
    for (int i = 0; i < numButterflies; i++) {
        auto path = std::make_shared<BezierPath>();
        
        // Adiciona pontos com variação aleatória em cada coordenada
        path->addPoint(glm::vec3(68.0f + e(rng),  5.0f + e(rng), 88.0f + e(rng)));
        path->addPoint(glm::vec3(36.0f + e(rng),  7.0f + e(rng), 79.0f + e(rng)));
        path->addPoint(glm::vec3(28.0f + e(rng),  6.0f + e(rng), 93.0f + e(rng)));
        path->addPoint(glm::vec3(-23.0f + e(rng), 7.0f + e(rng), 87.0f + e(rng)));
        path->addPoint(glm::vec3(-82.0f + e(rng), 5.0f + e(rng), 100.0f + e(rng)));
        path->addPoint(glm::vec3(-125.0f + e(rng),7.0f + e(rng), 65.0f + e(rng)));
        path->addPoint(glm::vec3(-129.0f + e(rng),5.0f + e(rng), 15.0f + e(rng)));
        path->addPoint(glm::vec3(-178.0f + e(rng),7.0f + e(rng), 13.0f + e(rng)));
        path->addPoint(glm::vec3(-185.0f + e(rng),5.0f + e(rng), -24.0f + e(rng)));
        path->addPoint(glm::vec3(-140.0f + e(rng),7.0f + e(rng), -51.0f + e(rng)));
        path->addPoint(glm::vec3(-90.0f + e(rng), 6.0f + e(rng), -45.0f + e(rng)));
        path->addPoint(glm::vec3(-35.0f + e(rng), 5.0f + e(rng), -81.0f + e(rng)));
        path->addPoint(glm::vec3(8.0f + e(rng),   5.0f + e(rng), -87.0f + e(rng)));
        path->addPoint(glm::vec3(70.0f + e(rng),  7.0f + e(rng), -66.0f + e(rng)));
        path->addPoint(glm::vec3(132.0f + e(rng), 5.0f + e(rng), -50.0f + e(rng)));
        path->addPoint(glm::vec3(196.0f + e(rng), 5.0f + e(rng), -13.0f + e(rng)));
        path->addPoint(glm::vec3(103.0f + e(rng), 5.0f + e(rng), 72.0f + e(rng)));
        path->addPoint(glm::vec3(57.0f + e(rng),  7.0f + e(rng), 50.0f + e(rng)));
        
        // Opcional: fecha o loop adicionando novamente o primeiro ponto
        path->addPoint(glm::vec3(68.0f + e(rng), 5.0f + e(rng), 88.0f + e(rng)));
        
        // Cria a borboleta com o caminho individual
        auto butterfly = std::make_shared<ButterflyNPC>(
            path,
            glm::vec3(0.0f),  // posição inicial (será sobrescrita)
            glm::vec3(0.0f),  // rotação
            glm::vec3(0.8f)   // escala
        );
        
        m_butterflyNPCs.push_back(butterfly);
    }
}

void Scene::buildDuckNPCs() {
    int count = 6;

    // Gerador aleatório e distribuição com variação maior
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> e(-12.0f, 12.0f);  // afasta mais os patos entre si

    // Centro do lago (elíptico) no plano XZ
    const glm::vec2 center(-30.0f, 0.0f);

    // Pontos base definem um percurso elíptico/oval dentro do lago
    // (todos com Y fixo entre -1 e 1)
    std::vector<glm::vec3> basePoints = {
        // Início no lado leste
        glm::vec3(center.x + 50.0f, -3.5f, center.y +  0.0f),
        // Norte
        glm::vec3(center.x + 30.0f,  -3.0f, center.y + 40.0f),
        // Oeste
        glm::vec3(center.x - 40.0f,  -3.0f, center.y + 30.0f),
        // Sudoeste
        glm::vec3(center.x - 50.0f, -3.5f, center.y - 20.0f),
        // Sul
        glm::vec3(center.x - 10.0f,  -3.0f, center.y - 45.0f),
        // Sudeste
        glm::vec3(center.x + 40.0f,  -3.0f, center.y - 30.0f),
        // Fecha o ciclo (volta ao primeiro)
        glm::vec3(center.x + 50.0f, -3.5f, center.y +  0.0f)
    };

    for (int i = 0; i < count; ++i) {
        auto path = std::make_shared<BezierPath>();

        // Para cada ponto base, adiciona um deslocamento aleatório em X e Z
        for (const auto& pt : basePoints) {
            float dx = e(rng);
            float dz = e(rng);
            glm::vec3 newPt = pt + glm::vec3(dx, 0.0f, dz);
            path->addPoint(newPt);
        }

        // Variação na escala (para dar diversidade)
        float s = 0.5f + abs(e(rng)) * 0.1f;

        auto duck = std::make_shared<DuckNPC>(
            path,
            glm::vec3(0.0f),  // posição inicial (será sobrescrita)
            glm::vec3(0.0f),  // rotação calculada pelo update
            glm::vec3(s)
        );

        m_duckNPCs.push_back(duck);
    }
}