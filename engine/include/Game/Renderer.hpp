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
class Mailbox;

//  NPCs
class ButterflyNPC;
class CarpNPC;

//  Partículas
class ParticleBurst;


class Renderer {
public:
    // Ciclo de vida
    void init(GLFWwindow* window);          // carrega shaders, texturas e modelos OBJ
    
    void initMinimal(GLFWwindow* window);
    void loadShaders();
    void loadTextures();
    void loadModels();
    void initSkybox();
    void drawSkybox(const glm::vec3& sunDir, float timeOfDay);
    void drawLoadingScreen(float progress);
    void drawMenu();

    void shutdown();

    // Frame
    void beginFrame(const Camera& camera);
    void endFrame(GLFWwindow* window);

    // Draw calls
    void drawBird   (Bird& bird, bool standing);
    void drawTerrain(Terrain& terrain);
    void drawRocks  (std::vector<std::shared_ptr<ProceduralRock>>& rocks);
    void drawRings  (std::vector<std::shared_ptr<Ring>>& rings);
    void drawLetter (Letter& letter);
    void drawMailbox(Mailbox& mailbox);
    void drawParticles(const std::vector<std::unique_ptr<ParticleBurst>>& bursts);

    void drawObjects(std::vector<std::shared_ptr<StaticObject>>& objects);

    void drawParabola(const std::vector<glm::vec3>& points, bool active);

    //  NPCs
    void drawButterflyNPC(ButterflyNPC& npc);
    void drawCarpNPC(CarpNPC& npc);

    // Utilitários
    void setWireframe(bool enabled);

    // IDs de objeto — espelham os #define do código original
    enum ObjectId : int {
        OBJ_SPHERE      = 0,
        OBJ_BUNNY       = 1,
        OBJ_PLANE       = 2,

        OBJ_TREE        = 3,
        OBJ_FLOWER      = 15,

        OBJ_BIRD        = 4,
        OBJ_BIRD2       = 5,
        OBJ_TARGET      = 6,

        OBJ_ROCK        = 7,

        OBJ_RING        = 8,
        OBJ_BUILDING    = 9,

        OBJ_LETTER      = 10,

        OBJ_HOUSE       = 11,
        OBJ_MAILBOX     = 12,

        //  NPCs
        OBJ_BUTTERFLY   = 13,
        OBJ_CARP        = 14,


        //  Objetos estáticos
        //  ROCHAS (1-5, HUGE_ROCK)
        OBJ_ROCK_1      = 16,
        OBJ_ROCK_2      = 17,
        OBJ_ROCK_3      = 18,
        OBJ_ROCK_4      = 19,
        OBJ_ROCK_5      = 20,
        OBJ_ROCK_6      = 21,
        OBJ_ROCK_7      = 22,
        OBJ_ROCK_8      = 23,
        OBJ_HUGE_ROCK   = 24,

        OBJ_LINE        = 99
    };

    //  Acessadores para matrizes de view e projeção
    glm::mat4 getViewMatrix() const { return m_view; }
    glm::mat4 getProjectionMatrix() const { return m_projection; }

    //  Partículas
    void initParticles();
    GLuint createParticlesProgram();
    GLuint m_particleProgram = 0;
    GLuint m_particleVAO     = 0;
    GLuint m_particleVBO     = 0;
private:
    GLuint    m_program           = 0;
    GLuint    m_modelUniform      = 0;
    GLuint    m_viewUniform       = 0;
    GLuint    m_projUniform       = 0;
    GLuint    m_objectIdUniform   = 0;

    glm::mat4 m_view              = glm::mat4(1.0f);
    glm::mat4 m_projection        = glm::mat4(1.0f);

    GLuint    m_texSand           = 0;
    GLuint    m_texGrass          = 0;
    GLuint    m_texRock           = 0;
    GLuint    m_texSnow           = 0;

    // Helpers
    void        bindProgram()                          const;
    void        setObjectId(ObjectId id)               const;
    void        setModel(const glm::mat4& model)       const;
    DrawContext makeContext(ObjectId id);

    Skybox m_skybox;
};