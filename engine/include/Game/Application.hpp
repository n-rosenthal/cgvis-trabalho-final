
/**
 * @file    `Application.hpp`
 * @brief   Header para implementação da classe que lida
 *          com o ciclo de vida do jogo, em alto nível
 */

#pragma once

#include <glad/glad.h>
#include <ctime>
#include <GLFW/glfw3.h>
#include <memory>

//  Forward declarations
class AssetLoader;

#include "audio/AudioManager.hpp"
#include "Game/Window.hpp"
#include "Game/Renderer.hpp"
#include "Game/Scene.hpp"
#include "Terrain/Skybox.hpp"

// Globais de controle de dia/noite (definidas em WindowCallbacks.cpp)
extern bool g_ManualDayNight;
extern bool g_DayTime;

//  Estado da aplicação
enum class GameState {
    LOADING,
    MENU,
    PLAYING,
    PAUSED,
    END
};

class Application {
public:
    void init(const char* title, int w, int h);
    void run();

private:
    void processFrame(float dt);
    float updateDayNight();

    Window   m_window;
    Renderer m_renderer;
    Scene    m_scene;
    std::unique_ptr<AssetLoader> m_loader;
    float    m_lastTime = 0.0f;

    Skybox      m_skybox;

    //  estado atual do jogo/aplicação
    GameState m_state = GameState::LOADING;

    //  processadores de momentos da aplicação
    void processLoading(float dt);
    void processMenu(float dt);
    void processPlaying(float dt);

    //  renderizadores de momentos da aplicação
    void renderLoading();
    void renderMenu();
};