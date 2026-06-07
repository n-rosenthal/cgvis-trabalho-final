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

#include "Game/Camera.hpp"
#include "Game/Bird.hpp"
#include "Objects/Letter.hpp"

#include "Terrain/Terrain.hpp"
#include "Objects/Ring.hpp"
#include "Objects/ProceduralRock.hpp"
#include "Objects/Tree.hpp"

class Renderer;  // forward

class Scene {
public:
    //  `build` invoca todos os métodos de construção dos objetos
    void build();

    void update(float dt, GLFWwindow* w);
    void resolveCollisions();
    void draw(Renderer& r);

    //  Métodos de construção dos objetos
    void buildTerrain();
    void buildTrees();
    void buildRocks();
    void buildRings();
    void buildLetter();
    


    //  Acessadores para os objetos da cena
    const std::optional<Bird>& getBird() const { return m_bird; }
    const std::unique_ptr<Terrain>& getTerrain() const { return m_terrain; }
    const std::vector<std::shared_ptr<Ring>>& getRings() const { return m_rings; }
    const std::vector<std::shared_ptr<ProceduralRock>>& getRocks() const { return m_rocks; }
    const std::vector<std::shared_ptr<Tree>>& getTrees() const { return m_trees; }
    const std::shared_ptr<Letter>& getLetter() const { return m_letter; }

    //  Acessador para a câmera
    const Camera& getCamera() const { return m_camera; }


private:
    // optional evita exigir construtor padrão de Terrain e Bird
    std::optional<Bird>                         m_bird;
    std::unique_ptr<Terrain>                    m_terrain;
    Camera                                      m_camera;


    std::vector<std::shared_ptr<Ring>>           m_rings;
    std::vector<std::shared_ptr<ProceduralRock>> m_rocks;
    std::vector<std::shared_ptr<Tree>>           m_trees;
    std::shared_ptr<Letter>                      m_letter;
};