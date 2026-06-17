/**
 * @file    `Renderer.cpp`
 */

#include "glad/glad.h"
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>

#include "Window/WindowCallbacks.hpp"
#include "Game/Renderer.hpp"
#include "Game/Camera.hpp"

#include "Objects/Assets.hpp"

#include "Game/Bird.hpp"
#include "Terrain/Terrain.hpp"
#include "Objects/ProceduralRock.hpp"
#include "Objects/Ring.hpp"
#include "Objects/Letter.hpp"

#include "Bezier/Butterfly/ButterflyNPC.hpp"
#include "Bezier/Carp/CarpNPC.hpp"

#include "Renderer/ShaderLoader.hpp"
#include "Loaders/TextureLoader.hpp"
#include "Loaders/ObjLoader.hpp"
#include "matrices.h"
#include "textrendering.hpp"

inline std::string asset_path(const std::string& sub) {
    return std::string("assets/") + sub;
}

namespace {
    constexpr float kFov  = glm::pi<float>() / 2.0f;
    constexpr float kNear = -0.1f;
    constexpr float kFar  = -512.0f;
}

// ── Ciclo de vida ─────────────────────────────────────────────────────────────

void Renderer::init(GLFWwindow* window){
    LoadShadersFromFiles();

    m_program         = g_GpuProgramID;
    m_modelUniform    = g_model_uniform;
    m_viewUniform     = g_view_uniform;
    m_projUniform     = g_projection_uniform;
    m_objectIdUniform = g_object_id_uniform;

    if (m_program == 0)
    {
        fprintf(stderr, "ERRO: shader inválido!\n");
        exit(1);
    }

    Assets::LoadAll();

    loadModels();

    TextRendering_Init(window);

    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
}

void Renderer::loadModels() const
{
    auto load = [](const ModelDefinition& model)
    {
        ObjModel obj(model.objFile.c_str());

        ComputeNormals(&obj);
        BuildTrianglesAndAddToVirtualScene(&obj);

        printf(
            "Loaded %s\n",
            model.objFile.c_str()
        );
    };

    load(Assets::TREE_1);
    load(Assets::BIRD_MODEL);
    load(Assets::BIRD_STANDING_MODEL);
    load(Assets::LETTER);
    load(Assets::BUTTERFLY);
    load(Assets::CARP);
    load(Assets::HOUSE);
    load(Assets::MAILBOX);

    printf("\n=== VIRTUAL SCENE ===\n");

    for(const auto& [name, obj] : g_VirtualScene)
    {
        printf(
            "Mesh: %s\n",
            name.c_str()
        );
    }

    printf("=====================\n");
}

void Renderer::shutdown() {}

// ── Helpers ───────────────────────────────────────────────────────────────────

void Renderer::bindProgram() const { glUseProgram(m_program); }

void Renderer::setObjectId(ObjectId id) const {
    glUniform1i(m_objectIdUniform, id);
}

void Renderer::setModel(const glm::mat4& model) const {
    glUniformMatrix4fv(m_modelUniform, 1, GL_FALSE, glm::value_ptr(model));
}

DrawContext Renderer::makeContext(ObjectId id) {
    return {
        .shader_program             = m_program,
        .model_uniform              = m_modelUniform,
        .view_uniform               = m_viewUniform,
        .projection_uniform         = m_projUniform,
        .object_id_uniform          = m_objectIdUniform,
        .diffuse_texture_uniform    = g_diffuse_texture_uniform,
        .object_id                  = (int)id
    };
}

// ── Frame ─────────────────────────────────────────────────────────────────────

void Renderer::beginFrame(const Camera& camera) {
    m_view       = camera.getViewMatrix();
    m_projection = Matrix_Perspective(kFov, g_ScreenRatio, kNear, kFar);


    bindProgram();
    glUniformMatrix4fv(m_viewUniform, 1, GL_FALSE, glm::value_ptr(m_view));
    glUniformMatrix4fv(m_projUniform, 1, GL_FALSE, glm::value_ptr(m_projection));
}

void Renderer::endFrame(GLFWwindow* window) {
    TextRendering_ShowEulerAngles(window);
    TextRendering_ShowProjection(window);
    TextRendering_ShowFramesPerSecond(window);
    TextRendering_ShowDebugPanel(window);
}

// ── Draw calls ────────────────────────────────────────────────────────────────

void Renderer::drawObjects(std::vector<std::shared_ptr<StaticObject>>& objects) {
    for(auto& object : objects)
        object->render(makeContext(OBJ_TREE));
}

void Renderer::drawBird(Bird& bird, bool standing) {
    if (standing) {
        bird.render(makeContext(OBJ_BIRD2));
    } else {
        bird.render(makeContext(OBJ_BIRD));
    }
}

void Renderer::drawTerrain(
    Terrain& terrain
)
{
    terrain.model(
        makeContext(OBJ_PLANE)
    );
}

void Renderer::drawRocks(std::vector<std::shared_ptr<ProceduralRock>>& rocks) {
    auto ctx = makeContext(OBJ_ROCK);
    for (const auto& rock : rocks)
        rock->render(ctx);   // GameObject::render -> RockDrawable::draw
}


void Renderer::drawRings(std::vector<std::shared_ptr<Ring>>& rings) {
    auto ctx = makeContext(OBJ_RING);
    glDisable(GL_CULL_FACE);
    for (const auto& ring : rings)
        ring->render(ctx, m_view);   // Ring::render injeta view no RingDrawable
    glEnable(GL_CULL_FACE);
}

void Renderer::drawLetter(Letter& letter) {
    letter.render(makeContext(OBJ_LETTER));
}


void Renderer::drawButterflyNPC(ButterflyNPC& npc) {
    npc.render(makeContext(OBJ_BUTTERFLY));
}

void Renderer::drawCarpNPC(CarpNPC& npc)
{
    npc.render(
        makeContext(OBJ_CARP)
    );
}


void Renderer::setWireframe(bool enabled) {
    glPolygonMode(GL_FRONT_AND_BACK, enabled ? GL_LINE : GL_FILL);
}