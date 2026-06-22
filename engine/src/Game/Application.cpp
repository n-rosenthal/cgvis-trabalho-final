/**
 * @file Application.cpp
 * @brief Implementação da classe que controla o ciclo de vida do jogo.
 */

#include "Game/Application.hpp"
#include "Game/AssetLoader.hpp"
#include "audio/AudioManager.hpp"
extern SoundManager g_Sound;

//
// ============================================================
// CÉU
// ============================================================
// O ciclo dia/noite foi removido: o skybox agora fornece um céu
// fixo (sempre o mesmo horário), constante ao longo do jogo.
//

namespace
{
    constexpr float kTimeOfDay = 0.5f;  // meio-dia, fixo
    const glm::vec3 kSunDir(0.3f, 0.5f, 0.2f);
}

//
// ============================================================
// INICIALIZAÇÃO
// ============================================================
//

void Application::init(
    const char* title,
    int w,
    int h
)
{
    //
    // Janela
    //
    m_window.init(
        title,
        w,
        h
    );

    //
    // Renderer mínimo
    //
    m_renderer.initMinimal(
        m_window.get()
    );

    //
    // Loader
    //
    m_loader =
        std::make_unique<AssetLoader>(
            m_renderer,
            m_scene
        );

    //
    // Começa na tela de loading
    //
    m_state =
        GameState::LOADING;

    g_Sound.init();

    m_lastTime =
        static_cast<float>(
            glfwGetTime()
        );
}

//
// ============================================================
// LOADING
// ============================================================
//

void Application::processLoading(float)
{
    if (!m_loader->finished())
    {
        m_loader->loadNextStep();
        return;
    }

    m_state =
        GameState::MENU;
}

void Application::renderLoading()
{
    glClear(
        GL_COLOR_BUFFER_BIT |
        GL_DEPTH_BUFFER_BIT
    );

    float progress =
        m_loader->progress();

    m_renderer.drawLoadingScreen(
        progress
    );
}

//
// ============================================================
// MENU
// ============================================================
//

void Application::processMenu(float)
{
    GLFWwindow* window =
        m_window.get();

    if (
        glfwGetKey(
            window,
            GLFW_KEY_ENTER
        ) == GLFW_PRESS
    )
    {
        m_state =
            GameState::PLAYING;
    }
}

void Application::renderMenu()
{
    glClear(
        GL_COLOR_BUFFER_BIT |
        GL_DEPTH_BUFFER_BIT
    );

    m_renderer.drawMenu();
}

//
// ============================================================
// JOGO
// ============================================================
//

void Application::processPlaying(float dt)
{
    m_scene.update(
        dt,
        m_window.get()
    );

    m_scene.resolveCollisions();
}

void Application::processFrame(float dt)
{
    switch(m_state)
    {
        case GameState::LOADING:
            processLoading(dt);
            renderLoading();
            break;

        case GameState::MENU:
            processMenu(dt);
            renderMenu();
            break;

        case GameState::PLAYING:
        {
            glClear(
                GL_DEPTH_BUFFER_BIT
            );

            processPlaying(dt);

            m_renderer.beginFrame(
                m_scene.getCamera()
            );

            m_renderer.drawSkybox(kSunDir, kTimeOfDay);

            m_scene.draw(
                m_renderer
            );

            m_renderer.endFrame(
                m_window.get()
            );

            break;
        }

        default:
            break;
    }
}

//
// ============================================================
// LOOP PRINCIPAL
// ============================================================
//

void Application::run()
{
    while(
        !glfwWindowShouldClose(
            m_window.get()
        )
    )
    {
        float current =
            static_cast<float>(
                glfwGetTime()
            );

        float dt =
            current - m_lastTime;

        m_lastTime =
            current;

        processFrame(dt);

        glfwSwapBuffers(
            m_window.get()
        );

        glfwPollEvents();
    }
}