/**
 * @file    `Window.cpp`
 * @brief   Implementação da classe de encapsulamento da janela do jogo (GLFW)
 */

#include "Game/Window.hpp"


//  Variáveis globais para obter info. do pássaro
//  e encaminhá-las para menu de debug
glm::vec3 g_DebugBirdPosition;
glm::vec3 g_DebugBirdVelocity;
float     g_DebugBirdSpeed;
glm::vec3 g_DebugBirdRotation;

//  varr. debug letter
glm::vec3 g_DebugLetterPosition;
glm::vec3 g_DebugLetterVelocity;
float     g_DebugLetterSpeed;
glm::vec3 g_DebugLetterRotation;

//  varr. debug. tempo  de execução dos métodos
//                      da aplicação principal
float g_debug_updateDayNight;
float g_debug_sceneUpdate;
float g_debug_sceneResolveCollisions;
float g_debug_rendererBeginFrame;
float g_debug_rendererDraw;
float g_debug_rendererEndFrame;

// Window.cpp
void Window::init(const char* title, int w, int h) {
    //  Inicialização GLFW
    if (!glfwInit()) { 
        fprintf(stderr, "glfwInit failed\n");
        std::exit(EXIT_FAILURE);
    }

    //  Configurações da janela
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //  Configurações p/ macOS
    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    //  Criação da janela e definição de um handle para esta
    m_handle = glfwCreateWindow(w, h, title, NULL, NULL);

    glfwSwapInterval(0);

    //  Verifica criação da janela
    if (!m_handle) {
        glfwTerminate();
        fprintf(stderr, "ERROR: glfwCreateWindow() failed.\n");
        std::exit(EXIT_FAILURE);
    }

    //  Definições de callbacks
    glfwSetKeyCallback(m_handle, KeyCallback);
    glfwSetMouseButtonCallback(m_handle, MouseButtonCallback);
    glfwSetCursorPosCallback(m_handle, CursorPosCallback);
    glfwSetScrollCallback(m_handle, ScrollCallback);
    glfwMakeContextCurrent(m_handle);

    //  Inicialização GLAD
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    //  Definições de callbacks para GLFW
    glfwSetFramebufferSizeCallback(m_handle, FramebufferSizeCallback);
    FramebufferSizeCallback(m_handle, w, h);

    //  Extração de dados da GPU via getters da GLAD
    const GLubyte *vendor = glGetString(GL_VENDOR);
    const GLubyte *renderer = glGetString(GL_RENDERER);
    const GLubyte *glversion = glGetString(GL_VERSION);
    const GLubyte *glslversion = glGetString(GL_SHADING_LANGUAGE_VERSION);
    printf("GPU: %s, %s, OpenGL %s, GLSL %s\n", vendor, renderer, glversion, glslversion);
};

/**
 * @brief   Verifica se a janela deve ser fechada
 * 
 * @return  true, se a janela deve ser fechada
 * @return  false, caso contrário
 */
bool Window::shouldClose() const {
    return glfwWindowShouldClose(m_handle);
};

/**
 * @brief   Swap buffers e poll events
 */
void Window::swapAndPoll() {
    glfwSwapBuffers(m_handle);
    glfwPollEvents();
};

/**
 * @brief Destruidor padrão para janela
 */
Window::~Window() {
    if (m_handle) glfwDestroyWindow(m_handle);
    glfwTerminate();
};