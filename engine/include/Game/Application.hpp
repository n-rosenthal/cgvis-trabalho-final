
/**
 * @file    `Application.hpp`
 * @brief   Header para implementação da classe que lida
 *          com o ciclo de vida do jogo, em alto nível
 */

#pragma once

#include <ctime>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Game/Window.hpp"
#include "Game/Renderer.hpp"
#include "Game/Scene.hpp"

// Globais de controle de dia/noite (definidas em WindowCallbacks.cpp)
extern bool g_ManualDayNight;
extern bool g_DayTime;

class Application {
public:
    void init(const char* title, int w, int h);
    void run();

private:
    void processFrame(float dt);
    void updateDayNight();

    Window   m_window;
    Renderer m_renderer;
    Scene    m_scene;
    float    m_lastTime = 0.0f;
};