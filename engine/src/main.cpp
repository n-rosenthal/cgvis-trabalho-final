//     Universidade Federal do Rio Grande do Sul
//             Instituto de Informática
//       Departamento de Informática Aplicada
//
//    INF01047 Computação Gráfica e Visualização I
//               Prof. Eduardo Gastal
//
//     CÓDIGO BASE PARA O TRABALHO FINAL
//

// Arquivos "headers" padrões de C podem ser incluídos em um
// programa C++, sendo necessário somente adicionar o caractere
// "c" antes de seu nome, e remover o sufixo ".h". Exemplo:
//    #include <stdio.h> // Em C
//  vira
//    #include <cstdio> // Em C++
//

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>

// Headers abaixo são específicos de C++
#include <set>
#include <map>
#include <stack>
#include <string>
#include <vector>
#include <limits>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>

// Headers das bibliotecas OpenGL
#include <glad/glad.h>   // Criação de contexto OpenGL 3.3
#include <GLFW/glfw3.h>  // Criação de janelas do sistema operacional
#include <GL/gl.h>

// Headers da biblioteca GLM: criação de matrizes e vetores.
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp> // glm::lookAt, glm::translate, etc.

// Headers da biblioteca para carregar modelos obj
#include <tiny_obj_loader.h>
#include <stb_image.h>

// Headers locais, definidos na pasta "include/"
#include "utils.h"
#include "matrices.h"
#include "Renderer/ShaderLoader.hpp"
#include "Window/WindowCallbacks.hpp"
#include "Window/InputCallbacks.hpp"
#include "textrendering.hpp"

//  Loader de modelos Wavefront .obj
#include "Loaders/ObjLoader.hpp"
#include "Loaders/TextureLoader.hpp"

// ÁUDIO e gerenciador de áudio
#include "audio/AudioManager.hpp"
SoundManager g_Sound;

//  Classe `Bird`: ave controlada pelo usuário
#include "Game/Bird.hpp"

// Classe `Tree`: árvores geradas aleatoriamente
#include "Objects/Tree.hpp"

// Classe `ProceduralTerrain`: terreno gerado proceduralmente
#include "Terrain/Terrain.hpp"
#include "Terrain/TerrainRegion.hpp"
#include "Terrain/TerrainType.hpp"

//  Classes de CAMADAS DE RELEVO para formação do TERRENO
#include "Terrain/SineLayer.hpp"
#include "Terrain/MountainLayer.hpp"
#include "Terrain/LakeLayer.hpp"

// Classes `ProceduralRock`: rochas geradas proceduralmente
#include "Objects/ProceduralRock.hpp"

// Classe `Ring`: anéis através dos quais o pássaro deve voar
#include "Objects/Ring.hpp"

// Class `Building`: construções/edifícios retangulares
#include "Objects/Building.hpp"

//  Sistema de Colisões
#include "Collision/CollisionSystem.hpp"

#define CHECK_GL_ERROR() do { \
    GLenum err = glGetError(); \
    if (err != GL_NO_ERROR) { \
        fprintf(stderr, "OpenGL error %d at %s:%d\n", err, __FILE__, __LINE__); \
    } \
} while(0)

/** função inline para obter o caminho para algum asset (textura, modelo) */
inline std::string asset_path(const std::string& subpath) {
    return std::string(ASSETS_DIR) + "/" + subpath;
}

// Pilha de matrizes de modelagem (não utilizada ativamente)
std::stack<glm::mat4>  g_MatrixStack;

// Variáveis globais de controle da câmera (definidas nos callbacks)
extern bool g_BirdView;

// Distância entre ponto e segmento (já implementada)
float distancePointSegment(glm::vec3 p, glm::vec3 a, glm::vec3 b);

int main(int argc, char* argv[])
{
    // Inicialização GLFW
    int success = glfwInit();
    if (!success) {
        fprintf(stderr, "ERROR: glfwInit() failed.\n");
        std::exit(EXIT_FAILURE);
    }
    g_Sound.init();

    glfwSetErrorCallback(ErrorCallback);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600,
        "INF01047 - Trabalho Final - Seu Cartao - Seu Nome", NULL, NULL);
    if (!window) {
        glfwTerminate();
        fprintf(stderr, "ERROR: glfwCreateWindow() failed.\n");
        std::exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, KeyCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    glfwSetCursorPosCallback(window, CursorPosCallback);
    glfwSetScrollCallback(window, ScrollCallback);
    glfwMakeContextCurrent(window);

    #ifdef GL_KHR_debug
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity,
                                GLsizei length, const GLchar* message, const void* userParam) {
            if (severity != GL_DEBUG_SEVERITY_NOTIFICATION) {
                fprintf(stderr, "OpenGL Debug: %s\n", message);
            }
        }, nullptr);
    #endif


    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    FramebufferSizeCallback(window, 800, 600);

    // Informações da GPU
    const GLubyte *vendor = glGetString(GL_VENDOR);
    const GLubyte *renderer = glGetString(GL_RENDERER);
    const GLubyte *glversion = glGetString(GL_VERSION);
    const GLubyte *glslversion = glGetString(GL_SHADING_LANGUAGE_VERSION);
    printf("GPU: %s, %s, OpenGL %s, GLSL %s\n", vendor, renderer, glversion, glslversion);


    

    #define SPHERE  0
    #define BUNNY   1
    #define PLANE   2
    #define BIRD    3
    #define ROCK    4
    #define RING    5
    #define BUILDING 6
    #define TREE    7

    // Carrega shaders
    LoadShadersFromFiles();
        printf(
        "GPU PROGRAM = %u\n",
        g_GpuProgramID
    );

    // Verifica se o programa de shader é válido
    if (g_GpuProgramID == 0) {
        fprintf(stderr, "ERRO: programa de shader inválido!\n");
        exit(1);
    } else {
        printf("Programa de shader ID: %u\n", g_GpuProgramID);
    }

    

    // Carrega texturas
    LoadTextureImage(asset_path("textures/red_brick_diff_1k.jpg").c_str());
    LoadTextureImage(asset_path("textures/rocky_terrain_02_diff_1k.jpg").c_str());

    // Carrega modelos OBJ para a cena virtual
    ObjModel spheremodel(asset_path("models/sphere.obj").c_str());
    ComputeNormals(&spheremodel);
    BuildTrianglesAndAddToVirtualScene(&spheremodel);

    ObjModel bunnymodel(asset_path("models/bunny.obj").c_str());
    ComputeNormals(&bunnymodel);
    BuildTrianglesAndAddToVirtualScene(&bunnymodel);

    ObjModel planemodel(asset_path("models/plane.obj").c_str());
    ComputeNormals(&planemodel);
    BuildTrianglesAndAddToVirtualScene(&planemodel);

    ObjModel birdmodel(asset_path("models/bird/0V3HJRW3DQ5QPF3J2O5PR4Z1M.obj").c_str());
    ComputeNormals(&birdmodel);
    BuildTrianglesAndAddToVirtualScene(&birdmodel);

    ObjModel tree1model(asset_path("models/trees/tree1/GenTree-103_AE3D_03122023-F1.obj").c_str());
    ComputeNormals(&tree1model);
    BuildTrianglesAndAddToVirtualScene(&tree1model);

    ObjModel tree2model(asset_path("models/trees/tree2/GenTree_105_AE3D_03122023-F2.obj").c_str());
    ComputeNormals(&tree2model);
    BuildTrianglesAndAddToVirtualScene(&tree2model);

    if (argc > 1) {
        ObjModel model(argv[1]);
        BuildTrianglesAndAddToVirtualScene(&model);
    }

    // Inicializa renderização de texto
    TextRendering_Init(window);

    // Configurações OpenGL
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    float last_frame_time = (float)glfwGetTime();


    Bird g_Bird;

    // =====================================================
    // TERRENO
    // =====================================================
    Terrain terrain(512, 512, 2.0f);

    // Materiais
    TerrainMaterial grassMaterial;    grassMaterial.id = 0;
    TerrainMaterial mountainMaterial; mountainMaterial.id = 1;
    TerrainMaterial waterMaterial;    waterMaterial.id = 2;

    // Região Sudoeste (campo suave)
    auto southwest = std::make_shared<TerrainRegion>(
        glm::vec2(0.0f, 0.0f), glm::vec2(256.0f, 256.0f),
        glm::vec3(0.20f, 0.80f, 0.20f), grassMaterial);
    southwest->addLayer(std::make_shared<SineLayer>(4.0f, 0.02f));
    terrain.addRegion(southwest);

    // Região Noroeste (montanhas)
    auto northwest = std::make_shared<TerrainRegion>(
        glm::vec2(0.0f, 256.0f), glm::vec2(256.0f, 512.0f),
        glm::vec3(0.45f, 0.45f, 0.45f), mountainMaterial);
    northwest->addLayer(std::make_shared<MountainLayer>(glm::vec2(50.0f, 350.0f), 120.0f, 60.0f));
    northwest->addLayer(std::make_shared<MountainLayer>(glm::vec2(65.0f, 365.0f), 140.0f, 110.0f));
    terrain.addRegion(northwest);

    // Região Nordeste (lago)
    auto northeast = std::make_shared<TerrainRegion>(
        glm::vec2(256.0f, 256.0f), glm::vec2(512.0f, 512.0f),
        glm::vec3(0.10f, 0.25f, 0.90f), waterMaterial);
    northeast->addLayer(std::make_shared<LakeLayer>(glm::vec2(400.0f, 400.0f), 100.0f, 100.0f));
    terrain.addRegion(northeast);

    // Região Sudeste (campo ondulado)
    auto southeast = std::make_shared<TerrainRegion>(
        glm::vec2(256.0f, 0.0f), glm::vec2(512.0f, 256.0f),
        glm::vec3(0.35f, 0.90f, 0.35f), grassMaterial);
    southeast->addLayer(std::make_shared<SineLayer>(16.0f, 0.05f));
    terrain.addRegion(southeast);

    terrain.generate();

    // =====================================================
    // ÁRVORES na região sudeste
    // =====================================================
    std::vector<std::shared_ptr<Tree>> southeastTrees;
    glm::vec3 southeastTrees_center(384.0f, 0.0f, 128.0f); // centro da região
    glm::vec3 southeastTrees_scale(1.0f);
    glm::vec3 southeastTrees_rotation(0.0f);
    int southeastTrees_num = 15;
    float southeastTrees_xdistance = 10.0f;
    float southeastTrees_zdistance = 10.0f;

    for (int t = 0; t < southeastTrees_num; t++) {
        float tree_x = southeastTrees_center.x - t * southeastTrees_xdistance;
        float tree_z = southeastTrees_center.z - t * southeastTrees_zdistance;
        glm::vec3 normal = terrain.getNormal(tree_x, tree_z);
        float height = terrain.getHeight(tree_x, tree_z);
        float pitch = atan2(normal.z, normal.y);
        float roll  = -atan2(normal.x, normal.y);
        glm::vec3 tree_rotation(pitch, 0.0f, roll);
        glm::vec3 tree_position(tree_x, height, tree_z);
        southeastTrees.push_back(std::make_shared<Tree>(tree_position, tree_rotation, southeastTrees_scale, 0));
    }

    // =====================================================
    // ROCHAS
    // =====================================================
    std::vector<ProceduralRock> g_Rocks;
    int rockCount = 50;
    srand((unsigned int)time(nullptr));
    for (int i = 0; i < rockCount; ++i) {
        float x = ((float)rand() / RAND_MAX - 0.5f) * 200.0f;
        float z = ((float)rand() / RAND_MAX - 0.5f) * 200.0f;
        float scale = 0.8f + ((float)rand() / RAND_MAX) * 3.5f;
        float y = terrain.getHeight(x, z);
        glm::vec3 normal = terrain.getNormal(x, z);
        glm::vec3 pos(x, y, z);
        pos += normal * scale * 0.5f;
        g_Rocks.emplace_back(pos, scale);
    }

    // =====================================================
    // ANÉIS
    // =====================================================
    std::vector<Ring> g_Rings;
    int n_rings = 25;
    for (int i = 0; i < n_rings; i++) {
        float x = ((rand() % 200) - 100);
        float z = ((rand() % 200) - 100);
        float y = terrain.getHeight(x, z) + 8.0f + (rand() % 20);
        g_Rings.emplace_back(glm::vec3(x, y, z), 2.5f);
    }

    // =====================================================
    // PRÉDIOS (opcional, comentado por enquanto)
    // =====================================================
    std::vector<Building> g_Buildings;
    int buildingsCount = 10;
    for (int i = 0; i < buildingsCount; ++i) {
        float x = ((float)rand() / RAND_MAX) * 300.0f;
        float z = ((float)rand() / RAND_MAX) * 300.0f;
        float width = 5.0f + ((float)rand() / RAND_MAX) * 10.0f;
        float depth = 5.0f + ((float)rand() / RAND_MAX) * 10.0f;
        float height = 20.0f + ((float)rand() / RAND_MAX) * 60.0f;
        g_Buildings.emplace_back(glm::vec3(x, height * 0.5f, z), width, height, depth);
    }

    // =====================================================
    // LOOP PRINCIPAL
    // =====================================================
    while (!glfwWindowShouldClose(window)) {
        // Tempo e delta time
        float current_frame_time = (float)glfwGetTime();
        float dt = current_frame_time - last_frame_time;
        last_frame_time = current_frame_time;

        // Atualiza lógica do pássaro
        g_Bird.update(dt, window);

        // Cor do céu (dia/noite)
        time_t now = time(0);
        struct tm* currentTime = localtime(&now);
        int hour = currentTime->tm_hour;
        bool actualDayTime = !(hour >= 18 || hour < 6);
        bool isDayTime = g_ManualDayNight ? g_DayTime : actualDayTime;
        if (isDayTime) glClearColor(0.9f, 0.9f, 1.0f, 1.0f);
        else glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // =========================================================
        // CÂMERA TERCEIRA PESSOA
        // =========================================================

        glm::vec3 birdPos = g_Bird.getPosition();
        glm::vec3 birdForward = g_Bird.getForward();
        glm::vec3 birdUp = g_Bird.getUp();

        float cameraDistance = 12.0f;
        float cameraHeight   = 4.0f;
        float lookAhead      = 8.0f;

        // posição desejada da câmera
        glm::vec3 desiredCameraPos =
            birdPos
            - birdForward * cameraDistance
            + birdUp * cameraHeight;

        // ponto observado
        glm::vec3 desiredTarget =
            birdPos
            + birdForward * lookAhead;

        // suavização
        static glm::vec3 cameraPos = desiredCameraPos;
        static glm::vec3 cameraTarget = desiredTarget;

        float smoothFactor =
            1.0f - exp(-5.0f * dt);

        cameraPos =
            glm::mix(
                cameraPos,
                desiredCameraPos,
                smoothFactor
            );

        cameraTarget =
            glm::mix(
                cameraTarget,
                desiredTarget,
                smoothFactor
            );

        glm::mat4 view =
            glm::lookAt(
                cameraPos,
                cameraTarget,
                glm::vec3(0,1,0)
            );


        // =========================================================
        // MATRIZ DE PROJEÇÃO (far plane aumentado)
        // =========================================================
        float nearplane = 0.1f;
        float farplane  = 2000.0f;
        glm::mat4 projection;
        if (g_UsePerspectiveProjection)
        {
            projection = glm::perspective(glm::radians(60.0f), g_ScreenRatio, nearplane, farplane);
        }
        else
        {
            float orthoSize = 30.0f;
            float left = -orthoSize * g_ScreenRatio;
            float right = orthoSize * g_ScreenRatio;
            float bottom = -orthoSize;
            float top = orthoSize;
            projection = glm::ortho(left, right, bottom, top, nearplane, farplane);
        }

        printf(
    "view[3] = %f %f %f %f\n",
    view[3][0],
    view[3][1],
    view[3][2],
    view[3][3]
);

        // Envia matrizes para a GPU
        glUseProgram(g_GpuProgramID); CHECK_GL_ERROR();

        // Envia matrizes para a GPU
        glUniformMatrix4fv(
            g_view_uniform,
            1,
            GL_FALSE,
            glm::value_ptr(view)
        ); CHECK_GL_ERROR();

        glUniformMatrix4fv(
            g_projection_uniform,
            1,
            GL_FALSE,
            glm::value_ptr(projection)
        ); CHECK_GL_ERROR();

        GLint p;
        glGetIntegerv(GL_CURRENT_PROGRAM, &p);
        printf("Current shader = %d\n", p);

        // =====================================================
        // DESENHA O PÁSSARO (usando GameObject::render)
        // =====================================================S

        DrawContext ctx;
        ctx.shader_program     = g_GpuProgramID;
        ctx.model_uniform      = g_model_uniform;
        ctx.view_uniform       = g_view_uniform;
        ctx.projection_uniform = g_projection_uniform;
        ctx.object_id_uniform  = g_object_id_uniform;
        ctx.object_id          = 3; // BIRD
        g_Bird.render(ctx);


        glGetIntegerv(GL_CURRENT_PROGRAM, &p);
        printf("After bird = %d\n", p);

        // printf("bird\nmodel_uniform = %d\n", g_model_uniform);
        // printf("view_uniform = %d\n", g_view_uniform);
        // printf("projection_uniform = %d\n", g_projection_uniform);
        // printf("object_id_uniform = %d\n", g_object_id_uniform);

        // Obtém colisor do pássaro (para detecção de colisões)
        birdPos = g_Bird.getPosition();
        glm::vec3 capsuleA = g_Bird.getCollider().p0;
        glm::vec3 capsuleB = g_Bird.getCollider().p1;
        float capsuleRadius = g_Bird.getCollider().radius;

        // Colisão com terreno
        float terrainHeight = terrain.getHeight(birdPos.x, birdPos.z);
        float birdBottomOffset = 0.6f;
        if (birdPos.y - birdBottomOffset <= terrainHeight) {
            glm::vec3 terrainNormal = terrain.getNormal(birdPos.x, birdPos.z);
            bool collided = g_Bird.onTerrainCollision(terrainHeight, terrainNormal);
            if (collided) {
                g_Sound.play("assets/audio/cartoon-boing-bouncy-big_F_major.wav");
            }
        }

        // =====================================================
        // TERRENO
        // =====================================================
        glm::mat4 model = glm::mat4(1.0f);
        glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
        glUniform1i(g_object_id_uniform, 2); // PLANE
        terrain.draw(g_model_uniform); CHECK_GL_ERROR();

        
        glGetIntegerv(GL_CURRENT_PROGRAM, &p);
        printf("After terrain = %d\n", p);



        // =====================================================
        // ROCHAS
        // =====================================================
        glUniform1i(g_object_id_uniform, 4); // ROCK
        for (auto& rock : g_Rocks) {
            rock.Draw(g_model_uniform);
            float distance = distancePointSegment(rock.getPosition(), capsuleA, capsuleB);
            if (distance < (capsuleRadius + rock.getCollisionRadius())) {
                g_Sound.play("assets/audio/cartoonish-stone-sfx-slow.wav");
                g_Bird.onCollision(rock.getPosition());
            }
        }


        glGetIntegerv(GL_CURRENT_PROGRAM, &p);
        printf("After rock = %d\n", p);

        // =====================================================
        // ÁRVORES
        // =====================================================
        DrawContext treeCtx = ctx;
        treeCtx.object_id = 7; // TREE
        for (auto& tree : southeastTrees) {
            tree->render(treeCtx);
            // Opcional: detecção de colisão com árvores (usar getColliders)
        }


        glGetIntegerv(GL_CURRENT_PROGRAM, &p);
        printf("After tree = %d\n", p);

        // printf("trees\nmodel_uniform = %d\n", g_model_uniform);
        // printf("view_uniform = %d\n", g_view_uniform);
        // printf("projection_uniform = %d\n", g_projection_uniform);
        // printf("object_id_uniform = %d\n", g_object_id_uniform);

        // =====================================================
        // ANÉIS
        // =====================================================
        glUniform1i(g_object_id_uniform, 5); // RING
        for (auto& ring : g_Rings) {
            ring.update(dt);
            ring.checkCollision(birdPos);
            glDisable(GL_CULL_FACE);
            ring.draw(g_model_uniform, view);
            glEnable(GL_CULL_FACE);
        }
        // Remove anéis coletados
        g_Rings.erase(std::remove_if(g_Rings.begin(), g_Rings.end(),
            [](const Ring& r) { return r.isDead(); }), g_Rings.end());

        
        glGetIntegerv(GL_CURRENT_PROGRAM, &p);
        printf("After rings = %d\n", p);
        // =====================================================
        // EDIFÍCIOS (comentado, pode ser reativado depois)
        // =====================================================
        // glUniform1i(g_object_id_uniform, 6);
        // for (auto& building : g_Buildings) { ... }

        // Desenha uma esfera de teste na posição do alvo
        glUniform1i(g_object_id_uniform, 0);   // SPHERE
        glm::mat4 modelSphere =
            glm::translate(
                glm::mat4(1.0f),
                birdPos + birdForward * 10.0f
            );
        glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(modelSphere));
        DrawVirtualObject("the_sphere"); CHECK_GL_ERROR();


        glGetIntegerv(GL_CURRENT_PROGRAM, &p);
        printf("After sphere = %d\n", p);
        // =====================================================
        // TEXTO NA TELA
        // =====================================================
        TextRendering_ShowEulerAngles(window);
        TextRendering_ShowProjection(window);
        TextRendering_ShowFramesPerSecond(window);
        TextRendering_ShowDebugPanel(window);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    glfwTerminate();
    return 0;
}

// Implementação da função distancePointSegment (já existia)
float distancePointSegment(glm::vec3 p, glm::vec3 a, glm::vec3 b) {
    glm::vec3 ab = b - a;
    float t = glm::dot(p - a, ab) / glm::dot(ab, ab);
    t = glm::clamp(t, 0.0f, 1.0f);
    glm::vec3 closest = a + t * ab;
    return glm::length(p - closest);
}

// Função que pega a matriz M e guarda a mesma no topo da pilha
void PushMatrix(glm::mat4 M)
{
    g_MatrixStack.push(M);
}

// Função que remove a matriz atualmente no topo da pilha e armazena a mesma na variável M
void PopMatrix(glm::mat4& M)
{
    if ( g_MatrixStack.empty() )
    {
        M = Matrix_Identity();
    }
    else
    {
        M = g_MatrixStack.top();
        g_MatrixStack.pop();
    }
}

// set makeprg=cd\ ..\ &&\ make\ run\ >/dev/null
// vim: set spell spelllang=pt_br :

