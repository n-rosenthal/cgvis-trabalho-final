/**
 * @file    Renderer.hpp
 * @brief   Gerencia shaders, uniforms e draw calls.
 */
#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

#include "Objects/Interfaces/Drawable.hpp"   // DrawContext
#include "Objects/StaticObject.hpp"
#include "Objects/ObjDrawable.hpp"
#include "Terrain/Skybox.hpp"

#include <vector>
#include <memory>

// Forward declarations
class Bird;
class Terrain;
class ProceduralRock;
class Ring;
class Camera;
class Letter;
class House;


//  NPCs
class ButterflyNPC;
class CarpNPC;

class Renderer {
public:
    // ── Ciclo de vida ─────────────────────────────────────────────────────────
    void init(GLFWwindow* window);   // carrega shaders, texturas e modelos OBJ
    void shutdown();

    // ── Frame ─────────────────────────────────────────────────────────────────
    void beginFrame(const Camera& camera);
    void endFrame(GLFWwindow* window);

    // ── Draw calls ────────────────────────────────────────────────────────────
    void drawBird   (Bird& bird, bool standing);
    void drawTerrain(Terrain& terrain);
    void drawRocks  (std::vector<std::shared_ptr<ProceduralRock>>& rocks);
    void drawRings  (std::vector<std::shared_ptr<Ring>>& rings);
    void drawLetter (Letter& letter);
    void drawHouses (const std::vector<std::shared_ptr<House>>& houses);

    void drawObjects(std::vector<std::shared_ptr<StaticObject>>& objects);


    //  NPCs
    void drawButterflyNPC(ButterflyNPC& npc);
    void drawCarpNPC(CarpNPC& npc);

    // ── Utilitários ───────────────────────────────────────────────────────────
    void setWireframe(bool enabled);

    // IDs de objeto — espelham os #define do código original
    enum ObjectId : int {
        OBJ_SPHERE      = 0,
        OBJ_BUNNY       = 1,
        OBJ_PLANE       = 2,

        OBJ_TREE        = 3,

        OBJ_BIRD        = 4,
        OBJ_BIRD2       = 5,
        OBJ_TARGET      = 6,

        OBJ_ROCK        = 7,

        OBJ_RING        = 8,
        OBJ_BUILDING    = 9,

        OBJ_LETTER      = 10,

        OBJ_HOUSE       = 11,

        //  NPCs
        OBJ_BUTTERFLY   = 12,
        OBJ_CARP        = 13
    };

    //  Acessadores para matrizes de view e projeção
    glm::mat4 getViewMatrix() const { return m_view; }
    glm::mat4 getProjectionMatrix() const { return m_projection; }

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