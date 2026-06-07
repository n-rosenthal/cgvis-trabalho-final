/**
 * @file    `Renderer.cpp`
 */

#include "glad/glad.h"
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>

#include "Window/WindowCallbacks.hpp"
#include "Game/Renderer.hpp"
#include "Game/Camera.hpp"
#include "Game/Bird.hpp"
#include "Terrain/Terrain.hpp"
#include "Objects/ProceduralRock.hpp"
#include "Objects/Ring.hpp"
#include "Objects/Tree.hpp"
#include "Objects/Letter.hpp"
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

void Renderer::init(GLFWwindow* window) {
    LoadShadersFromFiles();

    m_program         = g_GpuProgramID;
    m_modelUniform    = g_model_uniform;
    m_viewUniform     = g_view_uniform;
    m_projUniform     = g_projection_uniform;
    m_objectIdUniform = g_object_id_uniform;

    if (m_program == 0) { fprintf(stderr, "ERRO: shader inválido!\n"); exit(1); }

    LoadTextureImage(asset_path("textures/red_brick_diff_1k.jpg").c_str());
    LoadTextureImage(asset_path("textures/rocky_terrain_02_diff_1k.jpg").c_str());

    loadModels();
    TextRendering_Init(window);

    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
}

void Renderer::loadModels() const {
    printf("Carregando modelos de %s\n. . .", asset_path("models/") + "\n");
    auto load = [](const std::string& path) {
        ObjModel model(path.c_str());
        ComputeNormals(&model);
        BuildTrianglesAndAddToVirtualScene(&model);

        printf("Loaded %s\n", path.c_str());
    };

    //   Esfera
    load(asset_path("models/sphere.obj"));

    //  Coelho
    load(asset_path("models/bunny.obj"));

    //  Plano
    load(asset_path("models/plane.obj"));

    //  Pássaro ("bird")
    load(asset_path("models/bird/0V3HJRW3DQ5QPF3J2O5PR4Z1M.obj"));

    //  Pássaro ("bird_standing")
    load(asset_path("models/bird_standing/G1FXKUZIFSQHX0QERXO6AAO63.obj"));

    //  Árvores
    //      "Birch_Summer"
    load(asset_path("models/tree/Birch_Summer_1.obj"));

    //      "GenTree_105_AE3D_03122023-F2"
    load(asset_path("models/trees/GenTree-103_AE3D_03122023-F1.obj"));
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
        glUseProgram(m_program);

        GLint loc =
            glGetUniformLocation(
                m_program,
                "object_id"
            );

        printf(
            "objectId = %d, object_id location = %d\n",
            (int)id,
            loc
        );

        glUniform1i(
            loc,
            OBJ_PLANE
        );
    

    return {
        .shader_program     = m_program,
        .model_uniform      = m_modelUniform,
        .view_uniform       = m_viewUniform,
        .projection_uniform = m_projUniform,
        .object_id_uniform  = m_objectIdUniform,
        .object_id          = (int)id,
    };
};

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

void Renderer::drawBird(Bird& bird) {
    bird.render(makeContext(OBJ_BIRD));
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

void Renderer::drawTrees(std::vector<std::shared_ptr<Tree>>& trees) {
    auto ctx = makeContext(OBJ_TREE);
    for (const auto& tree : trees)
        tree->render(ctx);
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

void Renderer::setWireframe(bool enabled) {
    glPolygonMode(GL_FRONT_AND_BACK, enabled ? GL_LINE : GL_FILL);
}