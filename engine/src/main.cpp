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

// Headers da biblioteca GLM: criação de matrizes e vetores.
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

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

/** função inline para obter o caminho para algum asset (textura, modelo) 
    uso:
    	// carregar uma textura
    	LoadTextureImage(asset_path("textures/red_brick_diff_1k.jpg").c_str());
    	
    	// carregar um modelo
	ObjModel spheremodel(asset_path("models/sphere.obj").c_str());
*/
inline std::string asset_path(const std::string& subpath) {
    return std::string(ASSETS_DIR) + "/" + subpath;
}







// Declaração de funções utilizadas para pilha de matrizes de modelagem.
void PushMatrix(glm::mat4 M);
void PopMatrix(glm::mat4& M);


// Definimos uma estrutura que armazenará dados necessários para renderizar
// cada objeto da cena virtual.
struct SceneObject
{
    std::string  name;        // Nome do objeto
    size_t       first_index; // Índice do primeiro vértice dentro do vetor indices[] definido em BuildTrianglesAndAddToVirtualScene()
    size_t       num_indices; // Número de índices do objeto dentro do vetor indices[] definido em BuildTrianglesAndAddToVirtualScene()
    GLenum       rendering_mode; // Modo de rasterização (GL_TRIANGLES, GL_TRIANGLE_STRIP, etc.)
    GLuint       vertex_array_object_id; // ID do VAO onde estão armazenados os atributos do modelo
    glm::vec3    bbox_min; // Axis-Aligned Bounding Box do objeto
    glm::vec3    bbox_max;
};

// Abaixo definimos variáveis globais utilizadas em várias funções do código.

// A cena virtual é uma lista de objetos nomeados, guardados em um dicionário
// (map).  Veja dentro da função BuildTrianglesAndAddToVirtualScene() como que são incluídos
// objetos dentro da variável g_VirtualScene, e veja na função main() como
// estes são acessados.
std::map<std::string, SceneObject> g_VirtualScene;

// Pilha que guardará as matrizes de modelagem.
std::stack<glm::mat4>  g_MatrixStack;

// Razão de proporção da janela (largura/altura). Veja função FramebufferSizeCallback().
float g_ScreenRatio = 1.0f;

// Ângulos de Euler que controlam a rotação de um dos cubos da cena virtual
float g_AngleX = 0.0f;
float g_AngleY = 0.0f;
float g_AngleZ = 0.0f;

// "g_LeftMouseButtonPressed = true" se o usuário está com o botão esquerdo do mouse
// pressionado no momento atual. Veja função MouseButtonCallback().
bool g_LeftMouseButtonPressed = false;
bool g_RightMouseButtonPressed = false; // Análogo para botão direito do mouse
bool g_MiddleMouseButtonPressed = false; // Análogo para botão do meio do mouse

// Variáveis que definem a câmera em coordenadas esféricas, controladas pelo
// usuário através do mouse (veja função CursorPosCallback()). A posição
// efetiva da câmera é calculada dentro da função main(), dentro do loop de
// renderização.
float g_CameraTheta = 0.0f; // Ângulo no plano ZX em relação ao eixo Z
float g_CameraPhi = 0.0f;   // Ângulo em relação ao eixo Y
float g_CameraDistance = 6.0f; // Distância da câmera para a origem

// Variáveis que controlam rotação do antebraço
float g_ForearmAngleZ = 0.0f;
float g_ForearmAngleX = 0.0f;

// Variáveis que controlam translação do torso
float g_TorsoPositionX = 0.0f;
float g_TorsoPositionY = 0.0f;

// Variáveis que controlam posição e rotação do bunny
// float g_BunnyPositionX = 1.0f;
// float g_BunnyPositionY = 0.0f;
// float g_BunnyPositionZ = 0.0f;
// float g_BunnyRotationY = 0.0f;

// Variável que controla o tipo de projeção utilizada: perspectiva ou ortográfica.
bool g_UsePerspectiveProjection = true;

// Variável que controla se o texto informativo será mostrado na tela.
bool g_ShowInfoText = true;

// Variável que controla se o painel de depuração será mostrado na tela.
bool g_ShowDebugPanel = true;

// Variáveis que controlam o modo de dia/noite.
bool g_ManualDayNight = false;
bool g_DayTime = true;

// A variável abaixo controla se a câmera está em modo "bird view", ou seja, se ela
bool g_BirdView = false;



/**
 * BIRD: Ave controlada pelo usuário
*/
Bird g_Bird;

/**
 * Rochas geradas proceduralmente
*/
std::vector<ProceduralRock> g_Rocks;
int rockCount = 50;


/**
 * Prédios, construções retangulares 
 */
std::vector<Building> g_Buildings;
int buildingsCount = 10;


/**
 * @brief retorna a distância entre um ponto `p` e um segmento de reta `ab`
 * 
 * @param p 
 *      ponto
 * @param a 
 *      ponto A do segumento AB
 * @param b
 *      ponto B do segmento AB
 * @return float 
 */
float distancePointSegment(
    glm::vec3 p,
    glm::vec3 a,
    glm::vec3 b
) {
    glm::vec3 ab = b - a;

    float t =
        glm::dot(p - a, ab) /
        glm::dot(ab, ab);

    t = glm::clamp(t, 0.0f, 1.0f);

    glm::vec3 closest =
        a + t * ab;

    return glm::length(p - closest);
}


int main(int argc, char* argv[])
{
    // Inicializamos a biblioteca GLFW, utilizada para criar uma janela do
    // sistema operacional, onde poderemos renderizar com OpenGL.
    int success = glfwInit();
    if (!success)
    {
        fprintf(stderr, "ERROR: glfwInit() failed.\n");
        std::exit(EXIT_FAILURE);
    }

    // Inicialização do gerenciador de áudio (lib. miniaudio)
    g_Sound.init();

    // Definimos o callback para impressão de erros da GLFW no terminal
    glfwSetErrorCallback(ErrorCallback);

    // Pedimos para utilizar OpenGL versão 3.3 (ou superior)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    // Pedimos para utilizar o perfil "core", isto é, utilizaremos somente as
    // funções modernas de OpenGL.
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Criamos uma janela do sistema operacional, com 800 colunas e 600 linhas
    // de pixels, e com título "INF01047 ...".
    GLFWwindow* window;
    window = glfwCreateWindow(800, 600, "INF01047 - Trabalho Final - Seu Cartao - Seu Nome", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        fprintf(stderr, "ERROR: glfwCreateWindow() failed.\n");
        std::exit(EXIT_FAILURE);
    }

    // Definimos a função de callback que será chamada sempre que o usuário
    // pressionar alguma tecla do teclado ...
    glfwSetKeyCallback(window, KeyCallback);
    // ... ou clicar os botões do mouse ...
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    // ... ou movimentar o cursor do mouse em cima da janela ...
    glfwSetCursorPosCallback(window, CursorPosCallback);
    // ... ou rolar a "rodinha" do mouse.
    glfwSetScrollCallback(window, ScrollCallback);

    // Indicamos que as chamadas OpenGL deverão renderizar nesta janela
    glfwMakeContextCurrent(window);

    // Carregamento de todas funções definidas por OpenGL 3.3, utilizando a
    // biblioteca GLAD.
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    // Definimos a função de callback que será chamada sempre que a janela for
    // redimensionada, por consequência alterando o tamanho do "framebuffer"
    // (região de memória onde são armazenados os pixels da imagem).
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    FramebufferSizeCallback(window, 800, 600); // Forçamos a chamada do callback acima, para definir g_ScreenRatio.

    // Imprimimos no terminal informações sobre a GPU do sistema
    const GLubyte *vendor      = glGetString(GL_VENDOR);
    const GLubyte *renderer    = glGetString(GL_RENDERER);
    const GLubyte *glversion   = glGetString(GL_VERSION);
    const GLubyte *glslversion = glGetString(GL_SHADING_LANGUAGE_VERSION);

    printf("GPU: %s, %s, OpenGL %s, GLSL %s\n", vendor, renderer, glversion, glslversion);

    // Carregamos os shaders de vértices e de fragmentos que serão utilizados
    // para renderização. Veja slides 180-200 do documento Aula_03_Rendering_Pipeline_Grafico.pdf.
    //
    LoadShadersFromFiles();

    /* as texturas são armazenadas no diretório =assets/textures/= */
    // Carregamos duas imagens para serem utilizadas como textura
    std::string tex_path = std::string(ASSETS_DIR);
    LoadTextureImage(asset_path("textures/red_brick_diff_1k.jpg").c_str());		// TextureImage0
    LoadTextureImage(asset_path("textures/rocky_terrain_02_diff_1k.jpg").c_str());	// TextureImage1
    
    /* os objetos são armazenados no diretório =assets/models/= */
    // Construímos a representação de objetos geométricos através de malhas de triângulos
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


    if ( argc > 1 )
    {
        ObjModel model(argv[1]);
        BuildTrianglesAndAddToVirtualScene(&model);
    }

    // Inicializamos o código para renderização de texto.
    TextRendering_Init(window);

    // Habilitamos o Z-buffer. Veja slides 104-116 do documento Aula_09_Projecoes.pdf.
    glEnable(GL_DEPTH_TEST);

    // Habilitamos o Backface Culling. Veja slides 8-13 do documento Aula_02_Fundamentos_Matematicos.pdf, slides 23-34 do documento Aula_13_Clipping_and_Culling.pdf e slides 112-123 do documento Aula_14_Laboratorio_3_Revisao.pdf.
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    float last_frame_time = (float)glfwGetTime();

    // =====================================================
    // TERRENO
    // =====================================================
    //  Este é o território inteiro do mapa
    Terrain terrain(
        512,    // width
        512,    // depth
        2.0f    // spacing
    );

    // =====================================================
    // MATERIAIS do TERRENO
    // =====================================================

    TerrainMaterial grassMaterial;
    grassMaterial.id = 0;

    TerrainMaterial mountainMaterial;
    mountainMaterial.id = 1;

    TerrainMaterial waterMaterial;
    waterMaterial.id = 2;

    // =====================================================
    // REGIÃO SUDOESTE
    // Campo suave
    // =====================================================
    //  delimitação da REGIÃO do terreno
    auto southwest =
        std::make_shared<TerrainRegion>(
            glm::vec2(0.0f,   0.0f),            //  x_min, z_min
            glm::vec2(256.0f, 256.0f),          //  x_max, z_max
            glm::vec3(0.20f, 0.80f, 0.20f),     //  cor
            grassMaterial                       //  material
        );

    //  adição de uma CAMADA de RELEVO à REGIÃO SUDOESTE
    southwest->addLayer(
        std::make_shared<SineLayer>(
            4.0f,
            0.02f
        )
    );

    //  inclusão da REGIÃO ao TERRITÓRIO (terreno total)
    terrain.addRegion(southwest);

    // =====================================================
    // REGIÃO NOROESTE
    // Montanhas
    // =====================================================
    auto northwest =
        std::make_shared<TerrainRegion>(
            glm::vec2(0.0f,   256.0f),
            glm::vec2(256.0f, 512.0f),
            glm::vec3(0.45f, 0.45f, 0.45f),
            mountainMaterial
        );

    northwest->addLayer(
        std::make_shared<MountainLayer>(
            glm::vec2(
                50.0f,
                350.0f
            ),
            120.0f,
            60.0f
        )
    );

    northwest->addLayer(
        std::make_shared<MountainLayer>(
            glm::vec2(
                65.0f,
                365.0f
            ),
            140.0f,
            110.0f
        )
    );

    terrain.addRegion(northwest);

    // =====================================================
    // REGIÃO NORDESTE
    // Lago
    // =====================================================

    auto northeast =
        std::make_shared<TerrainRegion>(
            glm::vec2(256.0f, 256.0f),
            glm::vec2(512.0f, 512.0f),
            glm::vec3(0.10f, 0.25f, 0.90f),
            waterMaterial
        );

    northeast->addLayer(
        std::make_shared<LakeLayer>(
            glm::vec2(
                400.0f,
                400.0f
            ),
            100.0f,
            100.0f
        )
    );

    terrain.addRegion(northeast);

    // =====================================================
    // REGIÃO SUDESTE
    // Campo ondulado
    // =====================================================
    glm::vec2 southeast_min             = glm::vec2(256.0f, 0.0f);
    glm::vec2 southeast_max             = glm::vec2(512.0f, 256.0f);
    glm::vec3 southeast_color           = glm::vec3(0.35f, 0.90f, 0.35f);
    float southeast_sineLayer_amplitude = 16.0f;
    float southeast_sineLayer_frequency = 0.05;

    //  Definição da região southeast
    auto southeast =
        std::make_shared<TerrainRegion>(
            southeast_min,
            southeast_max,
            southeast_color,
            grassMaterial
        );

    //  Aplicação de uma camada sinusoidal à região southeast
    southeast->addLayer(
        std::make_shared<SineLayer>(
            southeast_sineLayer_amplitude,
            southeast_sineLayer_frequency
        )
    );

    //  Adicionar a região ao terreno
    terrain.addRegion(southeast);
    terrain.generate();

    //  ÁRVORES da região southeast
    std::vector<std::shared_ptr<Tree>> southeastTrees;
    glm::vec3 southeastTrees_center     = glm::vec3(
        southeast_min[0] + southeast_max[0] / 2.0f,
        0.0f,
        southeast_min[1] + southeast_max[1] / 2.0f
    );
    glm::vec3 southeastTrees_scale      = glm::vec3(
        1.0f,
        1.0f,
        1.0f
    );
    glm::vec3 southeastTrees_rotation   = glm::vec3(
        0.0f,
        0.0f,
        0.0f
    );

    int southeastTrees_num              = 15;
    float southeastTrees_xdistance      = 10.0f;
    float southeastTrees_zdistance      = 10.0f;
    
    //  para t entre [0, 15]
    for (int t = 0; t < southeastTrees_num; t++) {
        //  calcule a posição (x, z) da árvore
        float tree_x = southeastTrees_center[0] - t * southeastTrees_xdistance;
        float tree_z = southeastTrees_center[2] - t * southeastTrees_zdistance;

        //  extraia a altura do terreno e o vetor normal para a posição
        glm::vec3 normal = terrain.getNormal(tree_x, tree_z);
        float height     = terrain.getHeight(tree_x, tree_z);

        printf(
            "normal (%f,%f) -> (%f,%f,%f)\n",
            tree_x,
            tree_z,
            normal.x,
            normal.y,
            normal.z
        );

        printf(
            "altura (%f, %f) -> %f\n",
            tree_x,
            tree_z,
            height
        );

        float pitch = atan2(normal.z, normal.y);
        float roll  = -atan2(normal.x, normal.y);

        //  calcule a rotação da árvore, conforme a normal do terreno
        glm::vec3 tree_rotation(
            pitch,
            0.0f,
            roll
        );


        //  construa o vetor posição da árvore
        glm::vec3 tree_position = glm::vec3(
            tree_x,
            height,
            tree_z
        );

        //  defina a árvore e adicione ao vetor de árvores
        southeastTrees.push_back(
            std::make_shared<Tree>(
                tree_position,
                tree_rotation,
                southeastTrees_scale,
                0
            )
        );
    }




    //  ROCHAS geração procedural
    srand((unsigned int)time(nullptr));
    for (int i = 0; i < rockCount; ++i) {
        // =====================================
        // POSIÇÃO ALEATÓRIA NO TERRENO
        // =====================================

        float x =
            ((float)rand() / RAND_MAX - 0.5f)
            * 200.0f;

        float z =
            ((float)rand() / RAND_MAX - 0.5f)
            * 200.0f;

        // =====================================
        // ESCALA DA ROCHA
        // =====================================

        float scale =
            0.8f
            + ((float)rand() / RAND_MAX) * 3.5f;

        // =====================================
        // ALTURA DO TERRENO
        // =====================================

        float y =
            terrain.getHeight(x, z);

        // =====================================
        // NORMAL DO TERRENO
        // =====================================

        glm::vec3 normal =
            terrain.getNormal(x, z);

        // =====================================
        // POSIÇÃO FINAL
        // =====================================

        glm::vec3 pos(x, y, z);

        // levanta a rocha para evitar clipping
        pos += normal * scale * 0.5f;

        // =====================================
        // CRIA ROCHA
        // =====================================

        g_Rocks.emplace_back(
            pos,
            scale
        );
    }

    // ANÉIS
    std::vector<Ring> g_Rings;
    int n_rings = 25;
    for (int i = 0; i < n_rings; i++)
    {
        float x =
            ((rand() % 200) - 100);

        float z =
            ((rand() % 200) - 100);

        float y =
            terrain.getHeight(x, z)
            + 8.0f
            + (rand() % 20);

        g_Rings.emplace_back(
            glm::vec3(x,y,z),
            2.5f
        );
    }

    // PRÉDIOS/CONSTRUÇÕES
    for (int i = 0; i < buildingsCount; ++i){
        float x = ((float)rand() / RAND_MAX) * 300.0f;
        float z = ((float)rand() / RAND_MAX) * 300.0f;

        float width = 5.0f + ((float)rand() / RAND_MAX) * 10.0f;
        float depth = 5.0f + ((float)rand() / RAND_MAX) * 10.0f;
        float height = 20.0f + ((float)rand() / RAND_MAX) * 60.0f;

        g_Buildings.emplace_back(
            glm::vec3(x, height * 0.5f, z),
            width,
            height,
            depth
        );
    }


    // Ficamos em um loop infinito, renderizando, até que o usuário feche a janela
    while (!glfwWindowShouldClose(window))
    {
        // Aqui executamos as operações de renderização

        // Definimos a cor do "fundo" do framebuffer como branco.  Tal cor é
        // definida como coeficientes RGBA: Red, Green, Blue, Alpha; isto é:
        // Vermelho, Verde, Azul, Alpha (valor de transparência).
        // Conversaremos sobre sistemas de cores nas aulas de Modelos de Iluminação.
        //
        //           R     G     B     A

         // Obtém hora atual do computador
        time_t now = time(0);
        struct tm* currentTime = localtime(&now);
        int hour = currentTime->tm_hour;

        // Noite entre 18h e 6h, dia caso contrário
        bool actualDayTime = !(hour >= 18 || hour < 6);
        bool isDayTime = g_ManualDayNight ? g_DayTime : actualDayTime;


        if (isDayTime) {
            glClearColor(0.9f, 0.9f, 1.0f, 1.0f); // Dia: céu claro
        } else {
            glClearColor(0.1f, 0.1f, 0.2f, 1.0f); // Noite: céu escuro
        }

        // "Pintamos" todos os pixels do framebuffer com a cor definida acima,
        // e também resetamos todos os pixels do Z-buffer (depth buffer).
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Pedimos para a GPU utilizar o programa de GPU criado acima (contendo
        // os shaders de vértice e fragmentos).
        glUseProgram(g_GpuProgramID);

        float current_frame_time = (float)glfwGetTime();
        float dt = current_frame_time - last_frame_time;
        last_frame_time = current_frame_time;
        g_Bird.update(dt, window); 

        // =========================================================
        // CÂMERA THIRD PERSON ATRÁS DO PÁSSARO
        // =========================================================

        glm::vec3 birdPos = g_Bird.getPosition();
        glm::vec3 birdForward = g_Bird.getForward();

        // Distância da câmera atrás do pássaro
        float cameraDistance = 8.0f;

        // Altura da câmera
        float cameraHeight = 3.0f;

        // Posição da câmera:
        // fica atrás da direção do pássaro
        glm::vec3 cameraPos =
            birdPos
            - birdForward * cameraDistance
            + glm::vec3(0.0f, cameraHeight, 0.0f);

        // A câmera olha um pouco à frente do pássaro
        glm::vec3 target =
            birdPos
            + birdForward * 5.0f;

        // Vetor up
        glm::vec4 camera_up_vector =
            glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);

        // Conversão para vec4
        glm::vec4 camera_position_c =
            glm::vec4(cameraPos, 1.0f);

        glm::vec4 camera_lookat_l =
            glm::vec4(target, 1.0f);

        glm::vec4 camera_view_vector =
            camera_lookat_l - camera_position_c;

        // Computamos a matriz "View" utilizando os parâmetros da câmera para
        // definir o sistema de coordenadas da câmera.  Veja slides 2-14, 184-190 e 236-242 do documento Aula_08_Sistemas_de_Coordenadas.pdf.
        glm::mat4 view = Matrix_Camera_View(camera_position_c, camera_view_vector, camera_up_vector);

        // Agora computamos a matriz de Projeção.
        glm::mat4 projection;

        // Note que, no sistema de coordenadas da câmera, os planos near e far
        // estão no sentido negativo! Veja slides 176-204 do documento Aula_09_Projecoes.pdf.
        float nearplane = -0.1f;    // Posição do "near plane"
        float farplane  = -512.0f;  // Posição do "far plane"

        if (g_UsePerspectiveProjection)
        {
            // Projeção Perspectiva.
            // Para definição do field of view (FOV), veja slides 205-215 do documento Aula_09_Projecoes.pdf.
            float field_of_view = 3.141592 / 2.0f;
            projection = Matrix_Perspective(field_of_view, g_ScreenRatio, nearplane, farplane);
        }
        else
        {
            // Projeção Ortográfica.
            // Para definição dos valores l, r, b, t ("left", "right", "bottom", "top"),
            // PARA PROJEÇÃO ORTOGRÁFICA veja slides 219-224 do documento Aula_09_Projecoes.pdf.
            // Para simular um "zoom" ortográfico, computamos o valor de "t"
            // utilizando a variável g_CameraDistance.
            float t = 1.5f*g_CameraDistance/2.5f;
            float b = -t;
            float r = t*g_ScreenRatio;
            float l = -r;
            projection = Matrix_Orthographic(l, r, b, t, nearplane, farplane);
        }

        glm::mat4 model = Matrix_Identity(); // Transformação identidade de modelagem

        // Enviamos as matrizes "view" e "projection" para a placa de vídeo
        // (GPU). Veja o arquivo "shader_vertex.glsl", onde estas são
        // efetivamente aplicadas em todos os pontos.
        glUniformMatrix4fv(g_view_uniform       , 1 , GL_FALSE , glm::value_ptr(view));
        glUniformMatrix4fv(g_projection_uniform , 1 , GL_FALSE , glm::value_ptr(projection));

        #define SPHERE      0
        #define BUNNY       1
        #define PLANE       2
        #define BIRD        3 
        #define ROCK        4
        #define RING        5
        #define BUILDING    6
        #define TREE        7

        // =========================================================
        // PÁSSARO
        // =========================================================

        g_Bird.setModelMatrixUniform(
            g_model_uniform,
            view,
            projection
        );

        glUniform1i(g_object_id_uniform, BIRD);
        //  bird deverá ter um método `draw()` e todos os outros necessários
        //  a objetos que implementam interface `Drawable`
        // DrawVirtualObject("the_bird");

        //  Depois de desenhar o pássaro, vamos obter
        //  o vetor AB ao longo do comprimento do pássaro, e seu raio
        glm::vec3 capsuleA  = g_Bird.getCollider().p0;
        glm::vec3 capsuleB  = g_Bird.getCollider().p1;
        float capsuleRadius = g_Bird.getCollider().radius;

        // =========================================
        // PÁSSARO :: COLISÃO COM TERRENO
        // =========================================
        // Altura do terreno em relação ao pássaro
        float terrainHeight = terrain.getHeight(
                birdPos.x,
                birdPos.z
        );

        // offset do corpo do pássaro
        float birdBottomOffset = 0.6f;

        if (birdPos.y - birdBottomOffset <= terrainHeight) {
            glm::vec3 terrainNormal =
                terrain.getNormal(
                    birdPos.x,
                    birdPos.z
                );

            bool collided = g_Bird.onTerrainCollision(
                    terrainHeight,
                    terrainNormal
                );

            if (collided)
            {
                g_Sound.play(
                    "assets/audio/cartoon-boing-bouncy-big_F_major.wav"
                );
            }
        }
        // =========================================================
        // TERRENO
        // =========================================================

        model =
            Matrix_Translate(0.0f, -1.1f, 0.0f)
            * Matrix_Scale(10.0f, 1.0f, 10.0f);

        

        glUniformMatrix4fv(
            g_model_uniform,
            1,
            GL_FALSE,
            glm::value_ptr(model)
        );

        glUniform1i(g_object_id_uniform, PLANE);

        // agora desenha o terreno procedural
        terrain.draw(g_model_uniform);

        // =========================================================
        //  OBJETOS ESFÉRICOS   ::
        //                          ROCHAS PROCEDURAIS
        // =========================================================
        //  carregar sampler para ROCHA
        glUniform1i(g_object_id_uniform, ROCK);

        //  para cada rocha, faça...
        for (auto& rock : g_Rocks) {
            //  desenhe a rocha na cena virtual
            rock.Draw(g_model_uniform);

            //  calcule a distância entre o centro da rocha (p)
            //  e o segmento de reta AB (cápsula do pássaro)
            float distance = distancePointSegment(rock.getPosition(), capsuleA, capsuleB);

            //  verifique a distância contra a soma dos raios
            if (distance < (capsuleRadius + rock.getCollisionRadius())) {
                //  se houver, toque o som de colisão pássaro-pedra
                g_Sound.play("assets/audio/cartoonish-stone-sfx-slow.wav");

                // colisão do pássaro contra rocha
                g_Bird.onCollision(
                    rock.getPosition()
                );
            }
        }

        // =========================================================
        //  ÁRVORES
        // =========================================================
        //  árvores
        for (auto& tree : southeastTrees) {
                DrawContext context;

                context.model_uniform       = g_model_uniform;
                context.object_id_uniform   = g_object_id_uniform;
                context.object_id            = TREE;

                tree->draw(context, 0);
            }

        // =========================================================
        // ANÉIS
        // =========================================================

        glUniform1i(g_object_id_uniform, RING);

        for (auto& ring : g_Rings)
        {
            ring.update(dt);

            ring.checkCollision(
                g_Bird.getPosition()
            );

            //  Desabilita e habilita face culling
            glDisable(GL_CULL_FACE);
            ring.draw(g_model_uniform, view);
            glEnable(GL_CULL_FACE);
        }

        // =========================================================
        // EDIFÍCIOS
        // =========================================================

        glUniform1i(
            g_object_id_uniform,
            BUILDING
        );

        for (auto& building : g_Buildings)
        {
            building.Draw(
                g_model_uniform
            );

            if (
                building.checkCollision(
                    birdPos,
                    g_Bird.getCollider().radius
                )
            )
            {
                g_Sound.play(
                    "assets/audio/building-hit.wav"
                );

                g_Bird.onCollision(
                    building.getPosition()
                );

                printf(
                    "colisão com edifício\n"
                );
            }
        }
        // =========================================================`


        // Imprimimos na tela os ângulos de Euler que controlam a rotação do
        // terceiro cubo.
        TextRendering_ShowEulerAngles(window);

        // Imprimimos na informação sobre a matriz de projeção sendo utilizada.
        TextRendering_ShowProjection(window);

        // Imprimimos na tela informação sobre o número de quadros renderizados
        // por segundo (frames per second).
        TextRendering_ShowFramesPerSecond(window);

        // Imprimimos o painel de depuração com métricas de voo
        TextRendering_ShowDebugPanel(window);

        // O framebuffer onde OpenGL executa as operações de renderização não
        // é o mesmo que está sendo mostrado para o usuário, caso contrário
        // seria possível ver artefatos conhecidos como "screen tearing". A
        // chamada abaixo faz a troca dos buffers, mostrando para o usuário
        // tudo que foi renderizado pelas funções acima.
        // Veja o link: https://en.wikipedia.org/w/index.php?title=Multiple_buffering&oldid=793452829#Double_buffering_in_computer_graphics
        glfwSwapBuffers(window);

        // Verificamos com o sistema operacional se houve alguma interação do
        // usuário (teclado, mouse, ...). Caso positivo, as funções de callback
        // definidas anteriormente usando glfwSet*Callback() serão chamadas
        // pela biblioteca GLFW.
        glfwPollEvents();

        // ANÉIS: remover os anéis já percorridos
        g_Rings.erase(
        std::remove_if(
            g_Rings.begin(),
            g_Rings.end(),
            [](const Ring& r)
            {
                return r.isDead();
            }
        ),
        g_Rings.end()
        );
    }

    // Finalizamos o uso dos recursos do sistema operacional
    glfwTerminate();

    // Fim do programa
    return 0;
}



// Shaders agora dentro de assets/shaders/
std::string vert_path = std::string(ASSETS_DIR) + "/shaders/shader_vertex.glsl";
std::string frag_path = std::string(ASSETS_DIR) + "/shaders/shader_fragment.glsl";



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



// Variáveis globais que armazenam a última posição do cursor do mouse, para
// que possamos calcular quanto que o mouse se movimentou entre dois instantes
// de tempo. Utilizadas no callback CursorPosCallback() abaixo.
double g_LastCursorPosX, g_LastCursorPosY;





// set makeprg=cd\ ..\ &&\ make\ run\ >/dev/null
// vim: set spell spelllang=pt_br :

