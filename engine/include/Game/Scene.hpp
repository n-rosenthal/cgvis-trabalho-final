/**
 *  @file   `Scene.hpp`
 *  @brief  Header para implementação da classe `Scene`
 */

#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <algorithm>
#include <vector>
#include <memory>
#include <optional>

#include "Game/Bird.hpp"
#include "Game/Camera.hpp"
#include "Game/SceneBuilder.hpp"
#include "Terrain/Terrain.hpp"
#include "Objects/Ring.hpp"
#include "Objects/ProceduralRock.hpp"
#include "Objects/Tree.hpp"

class Renderer;  // forward

class Scene {
public:
    void build();
    void update(float dt, GLFWwindow* w);
    void resolveCollisions();
    void draw(Renderer& r);


    const Camera& getCamera() const { return m_camera; }


private:
    // optional evita exigir construtor padrão de Terrain e Bird
    std::optional<Bird>         m_bird;
    std::unique_ptr<Terrain>    m_terrain;
    Camera                      m_camera;


    std::vector<std::shared_ptr<Ring>>           m_rings;
    std::vector<std::shared_ptr<ProceduralRock>> m_rocks;
    std::vector<std::shared_ptr<Tree>>           m_trees;

    SceneBuilder m_builder;
};