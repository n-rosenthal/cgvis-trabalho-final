/**
 * @file    `Window.hpp`
 * @brief   Header para a classe de encapsulamento da janela do jogo (GLFW)
 */

#pragma once
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <string>
#include <memory>
#include <functional>

//  Callbacks
#include "Window/InputCallbacks.hpp"
#include "Window/WindowCallbacks.hpp"

//  Variáveis globais para obter info. do pássaro
//  e encaminhá-las para menu de debug
extern glm::vec3 g_DebugBirdPosition;
extern glm::vec3 g_DebugBirdVelocity;
extern float     g_DebugBirdSpeed;
extern glm::vec3 g_DebugBirdRotation;

//  varr. debug LETTER
extern glm::vec3 g_DebugLetterPosition;
extern glm::vec3 g_DebugLetterVelocity;
extern float     g_DebugLetterSpeed;
extern glm::vec3 g_DebugLetterRotation;

//  varr. debug. tempo  de execução dos métodos
//                      da aplicação principal
extern float g_debug_updateDayNight;
extern float g_debug_sceneUpdate;
extern float g_debug_sceneResolveCollisions;
extern float g_debug_rendererBeginFrame;
extern float g_debug_rendererDraw;
extern float g_debug_rendererEndFrame;


class Window {
public:
    void        init(const char* title, int w, int h);
    bool        shouldClose() const;
    void        swapAndPoll();
    GLFWwindow* get() const { return m_handle; }
    ~Window();

private:
    GLFWwindow* m_handle = nullptr;
};