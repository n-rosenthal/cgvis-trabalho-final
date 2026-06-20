/**
 * @file    `Scene.cpp`
 * @brief   Implementação da classe `Scene`
 */

#include <glad/glad.h>
#include "Game/Scene.hpp"
#include "Game/Renderer.hpp"
#include "audio/AudioManager.hpp"
#include "Collision/CollisionSystem.hpp"

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
    return glm::length(p - (a + t * ab));
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
    if (glm::length(forward) < 0.001f) forward = glm::vec3(0.0f, 0.0f, -1.0f);
    forward = glm::normalize(forward);

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
                m_letterState = LetterState::Thrown;
                m_parabolaActive = false;
                m_throwProgress = 0.0f;
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

    m_camera.update(
        m_bird->getPositionNoBob(),   // posição base
        m_bird->getForwardNoBob(),    // direção para onde o pássaro olha, sem o roll de bob
        m_bird->getUpNoBob(),         // vetor "para cima", sem o roll de bob (não usado por Camera::update, mantido por clareza)
        dt
    );


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
            0.4f
        );
    }

    if (m_letterState == LetterState::Thrown && m_letterTrailBurst)
    {
        m_letterTrailEmitter.emitAt(
            *m_letterTrailBurst,
            m_letter->getPosition(),
            0.3f
        );
    }

    for(auto& ring : m_rings) {
        ring->update(dt);

        if (ring->collected()) {
            printf("Ring collected!\n");
            spawnBurst(
                ring->getPosition(),
                60,
                4.0f,
                1.0f
            );
        }
    }

    if (m_mailbox)
        m_mailbox->update(dt);


    // NPCs

    for(auto& npc : m_butterflyNPCs) {
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
            glm::vec3 offset = m_bird->getForward() * 2.0f - m_bird->getUp() * 0.5f;
            m_letter->setPosition(m_bird->getPosition() + offset);
            m_letter->setRotation(m_bird->getRotation());
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

        case LetterState::Thrown: {
            // Avança o progresso
            m_throwProgress += m_throwSpeed * dt;
            if (m_throwProgress >= 1.0f) {
                m_throwProgress = 1.0f;
                m_letterState = LetterState::OnGround;
                // Define a posição final (garantindo que fique no chão)
                glm::vec3 finalPos = m_parabolaEnd;
                m_letter->setPosition(finalPos);
                m_letter->setGroundPos(glm::vec2(finalPos.x, finalPos.z));
                // Reseta a oscilação
                m_letterOscillationTime = 0.0f;
                break;
            }

            // Interpola na curva de Bézier
            float t = m_throwProgress;
            glm::vec3 p = (1-t)*(1-t)*m_parabolaStart 
                        + 2*(1-t)*t*m_parabolaControl 
                        + t*t*m_parabolaEnd;
            m_letter->setPosition(p);

            // Rotação: orienta ao longo da curva (opcional)
            // Pode-se calcular a derivada para orientar, mas deixaremos fixa.

            break;
        }

        case LetterState::OnGround:
        {
            // --- Floating oscillation ---
            // Accumulate time
            m_letterOscillationTime += dt;

            // Get parameters from the letter
            glm::vec2 ground = m_letter->getGroundPos();
            float h = m_letter->getFloatHeight();
            float d = m_letter->getAmplitude();
            float phase = m_letter->getPhase();

            // Compute vertical offset: sin wave
            float omega = 1.5f;              // angular frequency (rad/s)
            float offset = d * sin(omega * m_letterOscillationTime + phase);

            // Get terrain height at the ground position
            float terrainY = m_terrain->getHeight(ground.x, ground.y);

            // Set new position
            float y = terrainY + h + offset;
            m_letter->setPosition(glm::vec3(ground.x, y, ground.y));

            // --- Continuous rotation around Y axis ---
            glm::vec3 rot = m_letter->getRotation();
            float rotSpeed = 0.8f;          // radians per second
            rot.y += rotSpeed * dt;
            // Keep rotation in range (optional)
            if (rot.y > 2.0f * M_PI) rot.y -= 2.0f * M_PI;
            m_letter->setRotation(rot);

            break;
        }
    };
};


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

    // --------------------------------------------------
    // Carta arremessada → acerta a mailbox?
    // --------------------------------------------------

    if(
        m_letter &&
        m_mailbox &&
        m_letterState == LetterState::Thrown
    )
    {
        constexpr float kMailboxHitRadius = 4.0f;

        float d =
            glm::length(
                m_letter->getPosition()
                -
                m_mailbox->getPosition()
            );

        if(d < kMailboxHitRadius)
        {
            // Entrega bem-sucedida: trava a carta na mailbox
            m_letterState =
                LetterState::OnGround;

            m_letter->setPosition(
                m_mailbox->getPosition()
                +
                glm::vec3(0.0f, 2.0f, 0.0f)
            );

            m_letter->setGroundPos(
                glm::vec2(
                    m_mailbox->getPosition().x,
                    m_mailbox->getPosition().z
                )
            );

            m_letterVelocity =
                glm::vec3(0.0f);

            m_throwProgress = 0.0f;

            m_mailbox->activate();

            g_Sound.play(
                "assets/audio/cartoon-boing-bouncy-big_F_major.wav"
            );

            spawnBurst(
                m_mailbox->getPosition()
                +
                glm::vec3(0.0f, 2.0f, 0.0f),
                80,
                5.0f,
                1.2f
            );

            printf("Carta entregue! Jogo vencido.\n");
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

    r.drawMailbox(*m_mailbox);

    // --- Desenha a parábola da carta ---
    r.drawParabola(m_parabolaPoints, m_parabolaActive);

    // Partículas
    GLint currentProgram;

    glGetIntegerv(
        GL_CURRENT_PROGRAM,
        &currentProgram
    );

    printf(
        "Current shader = %d\n",
        currentProgram
    );
    r.drawParticles(
        m_particleBursts
    );
    
}

void Scene::spawnBurst(
    const glm::vec3& position,
    int count,
    float speed,
    float lifetime
)
{
    m_particleBursts.push_back(
        std::make_unique<ParticleBurst>(
            position,
            count,
            speed,
            lifetime
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
        130.5,
        90.5,
        6.0f
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
                .position   = glm::vec3(x,y,z),
                .rotation   = rot,
                .scale      = glm::vec3(s,s,s),
                .colliders  = {
                    //  usa colisor esférico para `Assets::MARINE_PLANT`
                    std::make_shared<SphereCollider>(
                        glm::vec3(0.0f,0.0f,0.0f),      //  centro do colisor essférico
                        s/2.0f                          //  raio do colisor essférico
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
                //  centro: (h(x,z) * 1.1))
                glm::vec3(0.0f, s*1.1f, 0.0f),

                //  raio: (s * 0.55f)
                s * 0.55f
            );
        
        //  tag: define a consequência da colisão
        //  TreeCanopy: redução da velocidade, colisão "falsa"
        canopy->tag = ColliderTag::TreeCanopy;

        //  colisor do tronco
        auto trunk =
            //  o tronco recebe um colisor cilíndrico
            std::make_shared<CylindricalCollider>(
                //  centro: (h(x,z) * 0.5f))
                glm::vec3(0.0f, s*0.5f, 0.0f),

                //  raio: (s * 0.12f)
                s*0.12f,

                //  altura: s
                s
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
std::vector<StaticObjectDef>
Scene::generateBushes(int count) {
    //  inicializa vetor de resposta
    std::vector<StaticObjectDef> out;

    //  cria gerador aleatório
    std::mt19937 rng(43);

    //  cria distribuições aleatórias
    std::uniform_real_distribution<float>
        px(-300.f,300.f),
        pz(-300.f,300.f),
        rot(0.f,360.f),
        scale(0.2f,0.8f);


    //  modelos possíveis
    const ModelDefinition* bushObjects[] =
    {
        &Assets::STYLED_SHRUB,
        &Assets::SHRUB,
        &Assets::BUSH
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
            s/2.0f
        );
        collider->tag = ColliderTag::Shrub;

        if (
            slope > 0.5f    ||
            lake < 1.1f     ||
            lake > 1.9f
        )   continue;

        //  seleciona o modelo
        auto model = bushObjects[rng()%3];

        //  adiciona ao vetor de resposta
        out.emplace_back(
            StaticObjectDef{
                .model      = model,
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
std::vector<StaticObjectDef>
Scene::generateRocks(int count) {
    //  inicializa vetor de resposta
    std::vector<StaticObjectDef> out;

    //  cria gerador aleatório
    std::mt19937 rng(44);

    //  cria distribuições aleatórias
    std::uniform_real_distribution<float>
        px(-300.f,300.f),
        pz(-300.f,300.f),
        rot(0.f,360.f),
        scale(0.05f, 0.1f);

    //  modelos possíveis
    const ModelDefinition* rocks[] = {
        // &Assets::ROCK_1,
        &Assets::ROCK_2,
        // &Assets::ROCK_3,
        &Assets::ROCK_4,
        &Assets::ROCK_5
    };

    //  considerando cada modelo de `rocks`, em [0..size-1]
    while(out.size() < count) {
        //  gera posições aleatórias
        float x = px(rng);
        float z = pz(rng);
        float y = m_terrain->getHeight(x,z);

        //  rotação aleatória
        float r = rot(rng);

        //  escala aleatória
        float s = scale(rng);
        
        //  colisor para rochas: AABB
        //  sempre colisor esférico com tag `ColliderTag::Rock`
        //      colisão "falsa" (redução da velocidade)
        auto collider = std::make_shared<AABBCollider>(
            glm::vec3(-s/2.0f, -s/2.0f, -s/2.0f),
            glm::vec3(s/2.0f, s/2.0f, s/2.0f)
        );
        collider->tag = ColliderTag::Rock;

        //  utiliza distância ao lago para decidir se prosseguiremos ou não com o objeto atual
        float lake = lakeDistance(x,z);        
        if(lake < 1.5f || lake > 2.0f) continue;

        out.emplace_back(
            StaticObjectDef{
                .model      = rocks[rng() % 3], // 3 modelos
                .position   = glm::vec3(x,y,z),
                .rotation   = glm::vec3(0.0f,r,0.0f),
                .scale      = glm::vec3(s,s,s),
                .colliders  = { collider }
            }
        );
    }


    return out;
}

/**
 * @brief   Constrói os objetos estáticos do jogo
 */
void Scene::buildStaticObjects() {
    //  limpa objetos anteriores
    m_staticObjects.clear();

    //  inicializa vetor de objetos
    std::vector<StaticObjectDef> objs;

    //  invocações às funções de geração de objetos
    //      interwoven by chamadas à glfwPollEvents()
    auto trees      = generateTrees(30);        glfwPollEvents();
    auto bushes     = generateBushes(50);       glfwPollEvents();
    auto rocks      = generateRocks(45);        glfwPollEvents();
    auto lake_objs  = generateLakeObjects(100); glfwPollEvents();

    //  adiciona objetos ao vetor
    objs.insert(objs.end(), trees.begin(), trees.end());
    objs.insert(objs.end(), bushes.begin(), bushes.end());
    objs.insert(objs.end(), rocks.begin(), rocks.end());
    objs.insert(objs.end(), lake_objs.begin(), lake_objs.end());

    //  adiciona agora ao vetor de objetos da cena
    for (size_t i = 0; i < objs.size(); ++i) {
        //  extrai dados do definição
        const auto& [model, position, rotation, scale, colliders] = objs[i];

        //  cria objeto
        m_staticObjects.push_back(
            std::make_shared<StaticObject>(
                *model,
                position,
                rotation,
                scale,
                colliders
            )
        );

        // Mantém a janela responsiva durante a construção dos objetos
        if (i % 5 == 0)
            glfwPollEvents();
    }

    glfwPollEvents();

    //  Construção de outros objetos únicos
    //  Casa
    ModelDefinition* house_model = &Assets::HOUSE;
    glm::vec3 house_position = glm::vec3(245.0f, m_terrain->getHeight(245.0f, -5.0f) - 1.5f, -5.0f);
    glm::vec3 house_rotation = glm::vec3(0.0f, glm::radians(90.0f), 0.0f);
    glm::vec3 house_scale = glm::vec3(1.0f);

    //  colisor para casa : AABB
    //  sempre colisor esférico com tag `ColliderTag::House`
    //      colisão verdadeira
    auto house_collider = std::make_shared<AABBCollider>(
        glm::vec3(-4.0f, 0.0f, -4.0f),
        glm::vec3( 4.0f, 5.0f,  4.0f)
    );
    house_collider->tag = ColliderTag::House;

    //  cria objeto
    m_staticObjects.push_back(
        std::make_shared<StaticObject>(
            *house_model,
            house_position,
            house_rotation,
            house_scale,
            std::vector<std::shared_ptr<Collider>>
            {
                house_collider
            }
        )
    );
}



/**
 * @brief   Constrói a mailbox, objetivo
 */
void Scene::buildMailbox() {
    float x = 235.0f, z = -20.0f;
    m_mailbox = std::make_shared<Mailbox>(
        glm::vec3(x, m_terrain->getHeight(x, z), z),
        glm::vec3(0.0f, 1.0f, glm::radians(90.0f)),
        glm::vec3(2.0f)
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
void Scene::buildLetter() {
    float x = -250.0f, z = 2.0f;
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