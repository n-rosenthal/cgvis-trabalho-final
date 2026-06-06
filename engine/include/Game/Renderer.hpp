/**
 * @file    Renderer.hpp
 * @brief   Gerencia shaders, uniforms e draw calls.
 */
#pragma once

#include "Objects/Interfaces/Drawable.hpp"   // DrawContext

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

#include <vector>
#include <memory>

// Forward declarations
class Bird;
class Terrain;
class ProceduralRock;
class Ring;
class Tree;
class Camera;

class Renderer {
public:
    // ── Ciclo de vida ─────────────────────────────────────────────────────────
    void init(GLFWwindow* window);   // carrega shaders, texturas e modelos OBJ
    void shutdown();

    // ── Frame ─────────────────────────────────────────────────────────────────
    void beginFrame(const Camera& camera);
    void endFrame(GLFWwindow* window);

    // ── Draw calls ────────────────────────────────────────────────────────────
    void drawBird   (Bird& bird);
    void drawTerrain(Terrain& terrain);
    void drawRocks  (std::vector<std::shared_ptr<ProceduralRock>>& rocks);
    void drawTrees  (std::vector<std::shared_ptr<Tree>>& trees);
    void drawRings  (std::vector<std::shared_ptr<Ring>>& rings);

    // ── Utilitários ───────────────────────────────────────────────────────────
    void setWireframe(bool enabled);

    // IDs de objeto — espelham os #define do código original
    enum ObjectId : int {
        OBJ_PLANE    = 2,
        OBJ_BIRD     = 3,
        OBJ_ROCK     = 4,
        OBJ_RING     = 5,
        OBJ_BUILDING = 6,
        OBJ_TREE     = 7,
    };

private:
    GLuint    m_program           = 0;
    GLuint    m_modelUniform      = 0;
    GLuint    m_viewUniform       = 0;
    GLuint    m_projUniform       = 0;
    GLuint    m_objectIdUniform   = 0;

    glm::mat4 m_view              = glm::mat4(1.0f);
    glm::mat4 m_projection        = glm::mat4(1.0f);

    // Helpers
    void        bindProgram()                          const;
    void        setObjectId(ObjectId id)               const;
    void        setModel(const glm::mat4& model)       const;
    DrawContext makeContext(ObjectId id);

    void loadModels() const;
};