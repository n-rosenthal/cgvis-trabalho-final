#include "Game/AssetLoader.hpp"
#include <GLFW/glfw3.h>

AssetLoader::AssetLoader(
    Renderer& renderer,
    Scene& scene
)
    : m_renderer(renderer)
    , m_scene(scene)
{
}

void AssetLoader::loadNextStep()
{
    switch(m_step)
    {
        case LoadingStep::SHADERS:
            m_renderer.loadShaders();
            Assets::LoadAll();
            m_renderer.initParticles();
            m_step = LoadingStep::TEXTURES;
            break;

        case LoadingStep::TEXTURES:
            m_renderer.loadTextures();
            m_step = LoadingStep::MODELS;
            break;

        case LoadingStep::MODELS:
            m_renderer.loadModels();
            m_step = LoadingStep::TERRAIN;
            break;

        case LoadingStep::TERRAIN:
            m_scene.buildTerrain();
            m_scene.spawnBird();
            m_step = LoadingStep::STATIC_OBJECTS;
            break;

        case LoadingStep::STATIC_OBJECTS:
            m_scene.buildStaticObjects();
            glfwPollEvents();

            m_scene.buildRings();
            glfwPollEvents();

            m_scene.buildLetter();
            m_scene.buildMailbox();
            glfwPollEvents();

            m_scene.buildButterflyNPCs();
            m_scene.buildCarpNPCs();
            glfwPollEvents();

            m_scene.buildTrails();

            m_step = LoadingStep::DONE;    //  Construtor para trilhas de partículas emitidas
    void buildTrails();
            break;

        default:
            break;
    }
}

bool AssetLoader::finished() const
{
    return m_step == LoadingStep::DONE;
}

float AssetLoader::progress() const
{
    // 5 transições entre os 6 estágios (SHADERS..DONE)
    constexpr int kTotalSteps = 5;

    int current =
        static_cast<int>(m_step);

    return
        static_cast<float>(current) /
        static_cast<float>(kTotalSteps);
}