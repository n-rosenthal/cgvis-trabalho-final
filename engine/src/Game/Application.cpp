/**
 * @file Application.cpp
 * @brief Implementação da classe que controla o ciclo de vida do jogo.
 */

#include "Game/Application.hpp"
#include "Game/AssetLoader.hpp"

#include <ctime>

//
// ============================================================
// DIA / NOITE
// ============================================================
//

float Application::updateDayNight()
{
    static bool isDayTime = true;
    static float lastCheckTime = -1.0f;

    float current =
        static_cast<float>(glfwGetTime());

    if (current - lastCheckTime > 1.0f)
    {
        lastCheckTime = current;

        time_t now = time(nullptr);
        tm* t = localtime(&now);

        int hour = t->tm_hour;

        bool actualDayTime =
            !(hour >= 18 || hour < 6);

        isDayTime =
            g_ManualDayNight
            ? g_DayTime
            : actualDayTime;
    }

    if (isDayTime)
        glClearColor(
            0.9f,
            0.9f,
            1.0f,
            1.0f
        );
    else
        glClearColor(
            0.1f,
            0.1f,
            0.2f,
            1.0f
        );

    // timeOfDay simples, sem transição: 0.5 = meio-dia, 0.0 = meia-noite
    return isDayTime ? 0.5f : 0.0f;
}

//
// ============================================================
// INICIALIZAÇÃO
// ============================================================
//
extern SoundManager g_Sound;

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
    //  áudio
    //
    if (!g_Sound.init()) {
        fprintf(stderr, "Erro ao inicializar áudio.\n");
    }


    //
    // Começa na tela de loading
    //
    m_state =
        GameState::LOADING;

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
            float timeOfDay = updateDayNight();

            glClear(
                GL_COLOR_BUFFER_BIT |
                GL_DEPTH_BUFFER_BIT
            );

            processPlaying(dt);

            m_renderer.beginFrame(
                m_scene.getCamera()
            );

            // Sol fixo: alto ao meio-dia, abaixo do horizonte à noite
            glm::vec3 sunDir =
                (timeOfDay >= 0.5f)
                    ? glm::vec3(0.3f, 0.8f, 0.2f)
                    : glm::vec3(0.1f, -0.6f, 0.1f);

            m_renderer.drawSkybox(sunDir, timeOfDay);

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