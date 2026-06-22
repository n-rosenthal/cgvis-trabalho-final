/**
 * @file Renderer.cpp
 */

#include "Game/Renderer.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/type_ptr.hpp>

#include "Window/WindowCallbacks.hpp"
#include "Game/Camera.hpp"

#include "Objects/Assets.hpp"

#include "Game/Bird.hpp"
#include "Terrain/Terrain.hpp"
#include "Objects/ProceduralRock.hpp"
#include "Objects/Ring.hpp"
#include "Objects/Letter.hpp"
#include "Objects/Mailbox.hpp"

#include "Particles/ParticleBurst.hpp"

#include "Bezier/Butterfly/ButterflyNPC.hpp"
#include "Bezier/Carp/CarpNPC.hpp"
#include "Bezier/Duck/DuckNPC.hpp"

#include "Renderer/ShaderLoader.hpp"
#include "Loaders/TextureLoader.hpp"
#include "Loaders/ObjLoader.hpp"

#include "matrices.h"
#include "textrendering.hpp"

#include <cmath>
#include <cstdio>
#include <cstddef> // offsetof

struct ParticleVertex
{
    glm::vec3 position;
    glm::vec4 color;
};

namespace
{
    constexpr float kFov  = glm::pi<float>() / 2.0f;
    constexpr float kNear = -0.1f;
    constexpr float kFar  = -1024.0f;
}

// ============================================================================
// INITIALIZATION
// ============================================================================

void Renderer:: initMinimal(GLFWwindow* window)
{
    glfwPollEvents();

    LoadShadersFromFiles();

    m_program         = g_GpuProgramID;
    m_modelUniform    = g_model_uniform;
    m_viewUniform     = g_view_uniform;
    m_projUniform     = g_projection_uniform;
    m_objectIdUniform = g_object_id_uniform;

    if (m_program == 0)
    {
        fprintf(stderr, "ERRO: shader inválido!\n");
        std::exit(EXIT_FAILURE);
    }

    TextRendering_Init(window);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
}

void Renderer::loadShaders()
{
    // Os shaders já são carregados em initMinimal() (LoadShadersFromFiles),
    // antes da tela de loading começar a rodar. Este passo do AssetLoader
    // existe só para manter a barra de progresso com 5 estágios uniformes.
}

void Renderer::loadTextures()
{
    printf("\n=================================\n");
    printf("CARREGANDO TEXTURAS\n");
    printf("=================================\n");

    m_texSand =
        LoadTextureImage(
            "assets/textures/terrain/Texturelabs_Soil_126M.jpg",
            true
        );

    glfwPollEvents();

    m_texGrass =
        LoadTextureImage(
            "assets/textures/terrain/Texturelabs_Soil_135M.jpg",
            true
        );

    glfwPollEvents();

    m_texRock =
        LoadTextureImage(
            "assets/textures/terrain/Texturelabs_Stone_137M.jpg",
            true
        );

    glfwPollEvents();

    m_texSnow =
        LoadTextureImage(
            "assets/textures/terrain/Texturelabs_Water_156M.jpg",
            true
        );

    glfwPollEvents();
}

void Renderer::loadModels()
{
    auto load =
    [](const ModelDefinition& model)
    {
        glfwPollEvents();

        double start = glfwGetTime();

        printf(
            "\n[LOAD] %s\n",
            model.objFile.c_str()
        );

        ObjModel obj(model.objFile.c_str());

        double afterLoad = glfwGetTime();

        printf(
            "  OBJ parse: %.3f s\n",
            afterLoad - start
        );

        ComputeNormals(&obj);

        double afterNormals = glfwGetTime();

        printf(
            "  Normals: %.3f s\n",
            afterNormals - afterLoad
        );

        BuildTrianglesAndAddToVirtualScene(&obj);

        double afterGPU = glfwGetTime();

        printf(
            "  Scene build: %.3f s\n",
            afterGPU - afterNormals
        );

        printf(
            "  Total: %.3f s\n",
            afterGPU - start
        );

        glfwPollEvents();
    };

    double totalStart = glfwGetTime();

    printf("\n=================================\n");
    printf("CARREGANDO MODELOS\n");
    printf("=================================\n");

    // ------------------------------------------------------------------------
    // Rochas
    // ------------------------------------------------------------------------

    load(Assets::ROCK_1);
    load(Assets::ROCK_2);
    load(Assets::ROCK_3);
    load(Assets::ROCK_4);
    load(Assets::ROCK_5);
    load(Assets::ROCK_6);
    load(Assets::ROCK_7);
    load(Assets::ROCK_8);
    load(Assets::ROCK_9);

    // ------------------------------------------------------------------------
    // Vegetação
    // ------------------------------------------------------------------------

    load(Assets::OAK);

    load(Assets::STYLED_SHRUB);
    load(Assets::SHRUB);

    load(Assets::MARINE_PLANT);

    // ------------------------------------------------------------------------
    // Gameplay
    // ------------------------------------------------------------------------

    load(Assets::TREE_1);

    load(Assets::BIRD_MODEL);
    load(Assets::BIRD_STANDING_MODEL);

    load(Assets::LETTER);


    //  ------------------------------------------------------------------------
    // NPCs
    // ------------------------------------------------------------------------

    load(Assets::BUTTERFLY);
    load(Assets::DUCK);

    load(Assets::HOUSE);
    load(Assets::MAILBOX);

    printf("\n=== VIRTUAL SCENE ===\n");

    for (const auto& [name, obj] : g_VirtualScene)
    {
        printf(
            "Mesh: %s\n",
            name.c_str()
        );
    }

    printf("=====================\n");

    printf(
        "\nTotal model loading time: %.3f s\n",
        glfwGetTime() - totalStart
    );
}

void Renderer::init(GLFWwindow* window)
{
    initMinimal(window);

    Assets::LoadAll();

    loadTextures();
    loadModels();
    initParticles();
}

GLuint LoadParticleProgram()
{
    printf("Loading particle shaders...\n");

    GLuint vs =
        LoadShader_Vertex(
            "assets/shaders/particles_vertex.glsl"
        );

    printf("VS = %u\n", vs);

    GLuint fs =
        LoadShader_Fragment(
            "assets/shaders/particles_fragment.glsl"
        );

    printf("FS = %u\n", fs);

    GLuint prog =
        CreateGpuProgram(vs, fs);

    printf("Program = %u\n", prog);

    GLint ok;

    glGetShaderiv(vs, GL_COMPILE_STATUS, &ok);
    printf("VS compiled = %d\n", ok);

    glGetShaderiv(fs, GL_COMPILE_STATUS, &ok);
    printf("FS compiled = %d\n", ok);

    return prog;
}

void Renderer::initParticles() {
        m_particleProgram =
        LoadParticleProgram();

    // Necessário para que gl_PointSize, definido no vertex shader de
    // partículas, tenha efeito. Sem isso, pontos são desenhados com
    // tamanho fixo (tipicamente 1px) — efetivamente invisíveis.
    glEnable(GL_PROGRAM_POINT_SIZE);

    glGenVertexArrays(1, &m_particleVAO);
    glGenBuffers(1, &m_particleVBO);

    glBindVertexArray(m_particleVAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_particleVBO);

    // reserva espaço para até 10000 partículas
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(ParticleVertex) * 10000,
        nullptr,
        GL_DYNAMIC_DRAW
    );

    // posição
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(ParticleVertex),
        (void*)0
    );

    glEnableVertexAttribArray(0);

    // cor
    glVertexAttribPointer(
        1,
        4,
        GL_FLOAT,
        GL_FALSE,
        sizeof(ParticleVertex),
        (void*)offsetof(ParticleVertex, color)
    );

    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    GLint success;
        glGetProgramiv(
            m_particleProgram,
            GL_LINK_STATUS,
            &success
        );

        if(!success)
        {
            char info[1024];

            glGetProgramInfoLog(
                m_particleProgram,
                sizeof(info),
                nullptr,
                info
            );

            printf(
                "Particle shader link error:\n%s\n",
                info
            );
        }
}

void Renderer::initSkybox() {
    m_skybox.init();
    printf(
        "[Skybox] init() chamado, isReady() = %s\n",
        m_skybox.isReady() ? "true" : "false"
    );
}


void Renderer::shutdown()
{
    if(m_particleVBO)
    glDeleteBuffers(1, &m_particleVBO);

    if(m_particleVAO)
        glDeleteVertexArrays(1, &m_particleVAO);

    if(m_particleProgram)
        glDeleteProgram(m_particleProgram);
}

// ============================================================================
// HELPERS
// ============================================================================

void Renderer::bindProgram() const
{
    glUseProgram(m_program);
}

void Renderer::setObjectId(ObjectId id) const
{
    glUniform1i(
        m_objectIdUniform,
        id
    );
}

void Renderer::setModel(const glm::mat4& model) const
{
    glUniformMatrix4fv(
        m_modelUniform,
        1,
        GL_FALSE,
        glm::value_ptr(model)
    );
}

DrawContext Renderer::makeContext(ObjectId id)
{
    return {
        .shader_program          = m_program,
        .model_uniform           = m_modelUniform,
        .view_uniform            = m_viewUniform,
        .projection_uniform      = m_projUniform,
        .object_id_uniform       = m_objectIdUniform,
        .diffuse_texture_uniform = g_diffuse_texture_uniform,
        .object_id               = static_cast<int>(id)
    };
}

// ============================================================================
// FRAME
// ============================================================================

void Renderer::beginFrame(const Camera& camera)
{
    m_view =
        camera.getViewMatrix();

    m_projection =
        Matrix_Perspective(
            kFov,
            g_ScreenRatio,
            kNear,
            kFar
        );

    bindProgram();

    glUniformMatrix4fv(
        m_viewUniform,
        1,
        GL_FALSE,
        glm::value_ptr(m_view)
    );

    glUniformMatrix4fv(
        m_projUniform,
        1,
        GL_FALSE,
        glm::value_ptr(m_projection)
    );
}

void Renderer::endFrame(GLFWwindow* window)
{
    TextRendering_ShowEulerAngles(window);
    TextRendering_ShowProjection(window);
    TextRendering_ShowFramesPerSecond(window);
    TextRendering_ShowDebugPanel(window);
}

// ============================================================================
// DRAW CALLS
// ============================================================================
void Renderer::drawParabola(const std::vector<glm::vec3>& points, bool active) {
    if (!active || points.empty()) return;

    // Verifica se há um shader ativo
    GLuint currentProgram = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, (GLint*)&currentProgram);
    if (currentProgram == 0) {
        // Nenhum shader ativo – não desenhamos
        return;
    }

    // Obtém os uniforms do programa atual
    GLint locModel = glGetUniformLocation(currentProgram, "model");
    GLint locView  = glGetUniformLocation(currentProgram, "view");
    GLint locProj  = glGetUniformLocation(currentProgram, "projection");
    GLint locObjId = glGetUniformLocation(currentProgram, "object_id");

    // Define a matriz modelo como identidade (pontos já em world space)
    glm::mat4 model = glm::mat4(1.0f);
    if (locModel >= 0) glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));

    // view e projection presumimos que já estão configurados (pelo chamador)
    // Se necessário, configure‑os também (mas normalmente já estão)
    if (locObjId >= 0) glUniform1i(locObjId, 99);

    // Cria/usa VAO/VBO para a linha
    static GLuint vao = 0, vbo = 0;
    if (vao == 0) {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
    }

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(glm::vec3), points.data(), GL_DYNAMIC_DRAW);

    // Atributo de posição (layout 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    glLineWidth(3.0f);
    glDrawArrays(GL_LINE_STRIP, 0, (GLsizei)points.size());

    // Restaura estado (opcional)
    glBindVertexArray(0);
}


void Renderer::drawParticles(
    const std::vector<std::unique_ptr<ParticleBurst>>& bursts
)
{
    std::vector<ParticleVertex> vertices;

    for(const auto& burst : bursts)
    {
        for(const auto& p : burst->particles())
        {
            if(p.life <= 0.0f)
                continue;

            vertices.push_back({
                p.position,
                p.color
            });
        }
    }

    if(vertices.empty())
        return;

    glUseProgram(m_particleProgram);

    glUniformMatrix4fv(
        glGetUniformLocation(m_particleProgram, "view"),
        1,
        GL_FALSE,
        glm::value_ptr(m_view)
    );

    glUniformMatrix4fv(
        glGetUniformLocation(m_particleProgram, "projection"),
        1,
        GL_FALSE,
        glm::value_ptr(m_projection)
    );

    glBindVertexArray(m_particleVAO);

    glBindBuffer(
        GL_ARRAY_BUFFER,
        m_particleVBO
    );

    glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        vertices.size() * sizeof(ParticleVertex),
        vertices.data()
    );

    glEnable(GL_BLEND);
    glBlendFunc(
        GL_SRC_ALPHA,
        GL_ONE_MINUS_SRC_ALPHA
    );

    glDrawArrays(
        GL_POINTS,
        0,
        static_cast<GLsizei>(vertices.size())
    );

    glDisable(GL_BLEND);

    glBindVertexArray(0);

    glUseProgram(m_program);
}

void Renderer::drawObjects(
    std::vector<std::shared_ptr<StaticObject>>& objects
)
{
    auto ctx = makeContext(OBJ_TREE);

    for (auto& object : objects)
        object->render(ctx);
}

void Renderer::drawBird(
    Bird& bird,
    bool standing
)
{
    bird.render(
        makeContext(
            standing
                ? OBJ_BIRD2
                : OBJ_BIRD
        )
    );
}

void Renderer::drawSkybox(const glm::vec3& sunDir, float timeOfDay)
{
    glfwPollEvents();
    if (!m_skybox.isReady())
        return;
 
    m_skybox.draw(
        m_projection,
        m_view,
        sunDir,
        timeOfDay
    );

    glfwPollEvents();
 
    // O Skybox usa o próprio shader (glUseProgram interno); religa
    // o programa principal antes de continuar desenhando o resto da cena.
    bindProgram();
}

void Renderer::drawTerrain(Terrain& terrain)
{
    glBindSampler(1, 0);
    glBindSampler(2, 0);
    glBindSampler(3, 0);
    glBindSampler(4, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_texSand);
    glUniform1i(
        glGetUniformLocation(
            m_program,
            "texSand"
        ),
        1
    );

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_texGrass);
    glUniform1i(
        glGetUniformLocation(
            m_program,
            "texGrass"
        ),
        2
    );

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, m_texRock);
    glUniform1i(
        glGetUniformLocation(
            m_program,
            "texRock"
        ),
        3
    );

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, m_texSnow);
    glUniform1i(
        glGetUniformLocation(
            m_program,
            "texSnow"
        ),
        4
    );

    glActiveTexture(GL_TEXTURE0);

    terrain.model(
        makeContext(OBJ_PLANE)
    );
}

void Renderer::drawRocks(
    std::vector<std::shared_ptr<ProceduralRock>>& rocks
)
{
    auto ctx =
        makeContext(OBJ_ROCK);

    for (auto& rock : rocks)
        rock->render(ctx);
}

void Renderer::drawRings(
    std::vector<std::shared_ptr<Ring>>& rings
)
{
    auto ctx =
        makeContext(OBJ_RING);

    glDisable(GL_CULL_FACE);

    for (auto& ring : rings)
        ring->render(ctx, m_view);

    glEnable(GL_CULL_FACE);
}

void Renderer::drawLetter(Letter& letter)
{
    letter.render(
        makeContext(OBJ_LETTER)
    );
}

void Renderer::drawMailbox(Mailbox& mailbox)
{
    mailbox.render(
        makeContext(OBJ_MAILBOX)
    );
}

void Renderer::drawButterflyNPC(
    ButterflyNPC& npc
)
{
    npc.render(
        makeContext(OBJ_BUTTERFLY)
    );
}


void Renderer::drawDuckNPC(
    DuckNPC& npc
)
{
    npc.render(
        makeContext(OBJ_DUCK)
    );
};

void Renderer::setWireframe(bool enabled)
{
    glPolygonMode(
        GL_FRONT_AND_BACK,
        enabled
            ? GL_LINE
            : GL_FILL
    );
}

// ============================================================================
// UI SCREENS (LOADING / MENU)
// ============================================================================

namespace
{
    // Desenha um retângulo preenchido em coordenadas de tela (pixels),
    // origem no canto inferior esquerdo, sem precisar de shader/geometria
    // dedicados — usa glScissor + glClear isolados.
    void DrawScreenRect(
        int x,
        int y,
        int w,
        int h,
        float r,
        float g,
        float b
    )
    {
        glEnable(GL_SCISSOR_TEST);

        glScissor(x, y, w, h);

        glClearColor(r, g, b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glDisable(GL_SCISSOR_TEST);
    }
}

void Renderer::drawLoadingScreen(float progress)
{
    // fundo já foi limpo pelo Application::renderLoading()

    int winW, winH;
    glfwGetFramebufferSize(
        glfwGetCurrentContext(),
        &winW,
        &winH
    );

    // ----------------------------------------------------------
    // Barra de progresso (moldura + preenchimento)
    // ----------------------------------------------------------

    const int barW = static_cast<int>(winW * 0.5f);
    const int barH = 24;
    const int barX = (winW - barW) / 2;
    const int barY = winH / 2 - barH / 2;

    // moldura
    DrawScreenRect(
        barX - 3, barY - 3,
        barW + 6, barH + 6,
        0.85f, 0.85f, 0.85f
    );

    // fundo da barra (vazio)
    DrawScreenRect(
        barX, barY,
        barW, barH,
        0.15f, 0.15f, 0.15f
    );

    // preenchimento proporcional ao progresso
    progress = glm::clamp(progress, 0.0f, 1.0f);

    int fillW =
        static_cast<int>(barW * progress);

    if (fillW > 0)
    {
        DrawScreenRect(
            barX, barY,
            fillW, barH,
            0.3f, 0.7f, 0.3f
        );
    }

    // ----------------------------------------------------------
    // Texto
    // ----------------------------------------------------------

    GLFWwindow* window =
        glfwGetCurrentContext();

    // Nome da etapa atual, deduzido da fração de progresso.
    // (5 estágios uniformes: shaders, texturas, modelos, terreno, objetos)
    const char* stageName = "Inicializando...";

    if (progress >= 0.8f)      stageName = "Construindo objetos...";
    else if (progress >= 0.6f) stageName = "Gerando terreno...";
    else if (progress >= 0.4f) stageName = "Carregando modelos...";
    else if (progress >= 0.2f) stageName = "Carregando texturas...";
    else                       stageName = "Carregando shaders...";

    TextRendering_PrintString(
        window,
        stageName,
        -0.2f,
        0.15f,
        1.3f
    );

    char pct[16];
    snprintf(
        pct,
        sizeof(pct),
        "%d%%",
        static_cast<int>(progress * 100.0f)
    );

    TextRendering_PrintString(
        window,
        pct,
        -0.03f,
        -0.01f,
        1.2f
    );
}

void Renderer::drawMenu()
{
    float textscale = 2.0f;

    GLFWwindow* window =
        glfwGetCurrentContext();

    auto textWidth =
        [&](const std::string& str, float scale)
        {
            int width, height;
            glfwGetWindowSize(
                window,
                &width,
                &height
            );

            scale *= textscale;

            float sx =
                scale /
                static_cast<float>(width);

            float w = 0.0f;

            for (char c : str)
            {
                for (
                    size_t j = 0;
                    j < dejavufont.glyphs_count;
                    ++j
                )
                {
                    if (
                        dejavufont.glyphs[j].codepoint ==
                        static_cast<uint32_t>(c)
                    )
                    {
                        w +=
                            dejavufont.glyphs[j].advance_x
                            * sx;
                        break;
                    }
                }
            }

            return w;
        };

    auto printCentered =
        [&](const std::string& str,
            float y,
            float scale)
        {
            float w =
                textWidth(
                    str,
                    scale
                );

            TextRendering_PrintString(
                window,
                str,
                -w * 0.5f,
                y,
                scale
            );
        };

    // =====================================================
    // TÍTULO
    // =====================================================

    printCentered(
        "  CARRIER BIRD SIMULATOR",
        0.70f,
        2.6f
    );

    printCentered(
        "JOANA & NICOLAS",
        0.52f,
        1.6f
    );

    printCentered(
        "   Computacao Grafica e Visualizacao",
        0.38f,
        1.1f
    );

    printCentered(
        "INF01047 | 2026",
        0.28f,
        1.1f
    );

    // =====================================================
    // CONTROLES
    // =====================================================

    printCentered(
        "CONTROLES",
        0.05f,
        1.3f
    );

    printCentered(
        "  W / A / S / D  -  MOVIMENTO",
        -0.05f,
        1.0f
    );

    printCentered(
        "  ESPACO  -  BATER AS ASAS | SUBIR",
        -0.14f,
        1.0f
    );

    printCentered(
        "  SHIFT  -  DESCER",
        -0.23f,
        1.0f
    );

    printCentered(
        "  G  -  PEGAR OU ENTREGAR CARTA",
        -0.32f,
        1.0f
    );

    // =====================================================
    // OBJETIVO
    // =====================================================

    printCentered(
        "   ATRAVESSE OS ANEIS E ENTREGUE A CARTA",
        -0.46f,
        1.1f
    );

    // =====================================================
    // PISCAR ENTER
    // =====================================================

    static double startTime =
        glfwGetTime();

    double elapsed =
        glfwGetTime() -
        startTime;

    bool visible =
        fmod(elapsed, 1.0) < 0.6;

    if (visible)
    {
        printCentered(
            "  PRESSIONE ENTER PARA COMECAR",
            -0.70f,
            1.5f
        );
    }
}