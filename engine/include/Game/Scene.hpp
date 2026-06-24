/**
 *  @file       `Scene.hpp`
 *  @brief      Header para implementação da classe `Scene`
 *  @details    A classe `Scene` representa a cena virtual do jogo.
 *              É dividida em duas partes: construção e manutenção/atualização dos objetos.
 *              A construção é feita através do método `build()`, que instancia os objetos;
 *              A atualização é feita através do método `update()`, que atualiza os objetos a cada frame;
 */

#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <algorithm>
#include <vector>
#include <memory>
#include <optional>

//  Objetos estáticos da cena virtual
#include "Objects/StaticObject.hpp"

//  Colisores
#include "Objects/Interfaces/Collidable.hpp"
#include "Collision/SphereCollider.hpp"
#include "Collision/AABBCollider.hpp"
#include "Collision/CapsuleCollider.hpp"
#include "Collision/CylindricalCollider.hpp"

#include "Game/Camera.hpp"
#include "Game/Bird.hpp"
#include "Objects/Letter.hpp"

#include "Terrain/Terrain.hpp"
#include "Terrain/Skybox.hpp"

#include "Objects/Ring.hpp"
#include "Objects/ProceduralRock.hpp"

#include "Objects/Mailbox.hpp"

//  Objetos dinâmicos: NPCs
#include "Bezier/BezierMover.hpp"
#include "Bezier/Butterfly/ButterflyNPC.hpp"
#include "Bezier/Duck/DuckNPC.hpp"

//  Partículas
#include "Particles/ParticleBurst.hpp"
#include "Particles/TrailEmitter.hpp"
#include "Particles/Colors.hpp"

class Renderer;  // forward

// Camera mode determines which object the camera follows
enum class CameraMode {
    FollowBird,    // default: behind the bird
    FollowLetter,  // while letter is in flight
    WinScreen,     // letter hit the mailbox
};


//  Definição de um modelo estático na cena virtual
struct StaticObjectDef {
    //  Modelo per se (objFile, meshes, texturas etc.)
    const ModelDefinition* model;

    //  Tipo do StaticObject
    StaticObjectType type;

    //  Vetores posição, rotação, escala
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

    //  Vetor de colisores  
    std::vector<std::shared_ptr<Collider>> colliders = {};
};

class Scene {
public:
    //  Aceleração da gravidade
    static constexpr float GRAVITY = 9.81f;

    //  Inicializador de `Bird` na cena virtual
    void spawnBird();

    //  Atualização geral
    void update(float dt, GLFWwindow* w);

    //  Atualização do estado da carta
    void updateLetter(float dt, GLFWwindow* w);
    void computeParabola();

    void resolveCollisions();
    void draw(Renderer& r);

    //  Partículas
    void spawnBurst(
        const glm::vec3& position,
        int count,
        float speed,
        float lifetime,
        const glm::vec4& color = glm::vec4(1.0f)
    );

    //  Construtor para trilhas de partículas emitidas
    void buildTrails();
    

    //  Métodos de construção dos objetos
    void buildTerrain();
    void buildRings();
    void buildLetter();
    void buildMailbox();

    //  Constrói os objetos estáticos do jogo
    float lakeDistance(float x, float z) const;
    float terrainSlope(float x, float z) const;
    std::vector<StaticObjectDef> generateBushes(int count);
    std::vector<StaticObjectDef> generateTrees(int count);
    std::vector<StaticObjectDef> generateRocks(int count);
    std::vector<StaticObjectDef> generateLakeObjects(int count);
    void buildStaticObjects();

    void buildButterflyNPCs();
    void buildDuckNPCs();

    //  Acessadores para os objetos da cena
    const std::optional<Bird>& getBird() const { return m_bird; }
    const std::unique_ptr<Terrain>& getTerrain() const { return m_terrain; }
    const std::vector<std::shared_ptr<Ring>>& getRings() const { return m_rings; }
    const std::vector<std::shared_ptr<ProceduralRock>>& getRocks() const { return m_rocks; }
    const std::shared_ptr<Letter>& getLetter() const { return m_letter; }


    //  Acessador para a câmera
    const Camera& getCamera() const { return m_camera; }


    float m_collisionSoundCooldown = 0.0f;


private:
    std::vector<std::unique_ptr<ParticleBurst>>     m_particleBursts;

    // optional evita exigir construtor padrão de Terrain e Bird
    std::optional<Bird>                             m_bird;
    std::unique_ptr<Terrain>                        m_terrain;
    Camera                                          m_camera;

    //  Vetor de objetos estáticos (árvores, casa)
    std::vector<std::shared_ptr<StaticObject>>      m_staticObjects;

    std::vector<std::shared_ptr<Ring>>              m_rings;
    std::vector<std::shared_ptr<ProceduralRock>>    m_rocks;
    std::shared_ptr<Letter>                         m_letter;

    std::shared_ptr<Mailbox>                        m_mailbox;

    // -------- Camera --------
    CameraMode m_cameraMode = CameraMode::FollowBird;

    //  CARTA, PARÁBOLA
    //  Estado atual da carta
    LetterState m_letterState   = LetterState::OnGround;

    //  Velocidade da carta
    glm::vec3 m_letterVelocity  = glm::vec3(0.0f);

    //  Animação da carta
    float m_letterOscillationTime = 0.0f;

    bool m_parabolaActive = false;          // true enquanto G está pressionado
    std::vector<glm::vec3> m_parabolaPoints; // pontos para desenhar a curva
    glm::vec3 m_parabolaStart;              // ponto de partida (junto ao pássaro)
    glm::vec3 m_parabolaControl;            // ponto de controle (pico do arco)
    glm::vec3 m_parabolaEnd;                // ponto de impacto no chão
    float m_throwProgress = 0.0f;           // 0..1 progresso da animação
    float m_throwSpeed = 2.0f;              // velocidade de progresso (1/s)

    //  Emissor de partículas para `Bird`
    TrailEmitter m_birdTrailEmitter{
        0.04f,   // emitInterval: nova partícula a cada 0.04s (~25/s)
        0.5f,    // particleLifetime
        0.12f,   // particleSize
        glm::vec4(0.9f, 0.9f, 1.0f, 1.0f)  // cor: branco-azulado, esteira de ar
    };
    
    //  Emissor de partículas para `Letter`
    TrailEmitter m_letterTrailEmitter{
        0.03f,
        0.4f,
        0.1f,
        glm::vec4(1.0f, 0.9f, 0.5f, 1.0f)  // cor: dourado, "rastro mágico" da carta
    };

    //  Ponteiros aos bursts de partículas de `Bird` e `Letter`
    ParticleBurst* m_birdTrailBurst   = nullptr;
    ParticleBurst* m_letterTrailBurst = nullptr;

    //  NPCs
    std::vector<std::shared_ptr<ButterflyNPC>>  m_butterflyNPCs;
    std::vector<std::shared_ptr<DuckNPC>>       m_duckNPCs;
};