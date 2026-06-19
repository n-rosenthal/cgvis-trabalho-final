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
#include "Bezier/Carp/CarpNPC.hpp"

class Renderer;  // forward

/**
 * @brief   `LetterState` representa o estado da `Letter`
 * @details  correspondem aos estados possíveis da carta
 *              - `OnGround`: carta no chão, passível de ser pega pelo pássaro
 *              - `Carried`: carta sendo carregada pelo pássaro
 *              - `Falling`: carta caindo
 */
enum class LetterState {
    OnGround,
    Carried,
    Falling
};

class Scene {
public:
    //  Aceleração da gravidade
    static constexpr float GRAVITY = 9.81f;

    //  `build` invoca todos os métodos de construção dos objetos
    void build();

    //  Atualização geral
    void update(float dt, GLFWwindow* w);

    //  Atualização do estado da carta
    void updateLetter(float dt, GLFWwindow* w);

    void resolveCollisions();
    void draw(Renderer& r);

    //  Métodos de construção dos objetos
    void buildTerrain();
    void buildRings();
    void buildLetter();
    void buildMailbox();

    //  Constrói os objetos estáticos do jogo
    void buildStaticObjects();

    void buildButterflyNPCs();
    void buildCarpNPCs();

    //  Acessadores para os objetos da cena
    const std::optional<Bird>& getBird() const { return m_bird; }
    const std::unique_ptr<Terrain>& getTerrain() const { return m_terrain; }
    const std::vector<std::shared_ptr<Ring>>& getRings() const { return m_rings; }
    const std::vector<std::shared_ptr<ProceduralRock>>& getRocks() const { return m_rocks; }
    const std::shared_ptr<Letter>& getLetter() const { return m_letter; }


    //  Acessador para a câmera
    const Camera& getCamera() const { return m_camera; }


private:
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

    //  Estado atual da carta
    LetterState m_letterState   = LetterState::OnGround;

    //  Velocidade da carta
    glm::vec3 m_letterVelocity  = glm::vec3(0.0f);


    //  NPCs
    std::vector<std::shared_ptr<ButterflyNPC>>  m_butterflyNPCs;
    std::vector<std::shared_ptr<CarpNPC>>       m_carpNPCs;
};