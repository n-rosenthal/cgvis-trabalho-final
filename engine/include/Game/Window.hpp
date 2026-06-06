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