/**
 * @file    `Application.cpp`
 * @brief   Implementação da classe que lida com o ciclo de vida do jogo, em alto nível
 */

#include "Game/Application.hpp"

/**
 * @brief   Faz chamada de sistema para descobrir o timestamp
 *          do sistema atual e decide por iluminação diurna
 *          ou noturna
 */
void Application::updateDayNight() {
    static bool isDayTime = true;
    static float lastCheckTime = -1.0f;
    float current = (float)glfwGetTime();

    // Check system time only once per second to avoid expensive system calls every frame
    if (current - lastCheckTime > 1.0f) {
        lastCheckTime = current;
        time_t now = time(0);
        struct tm* t = localtime(&now);
        int hour = t->tm_hour;

        // Dia = entre 6h e 18h
        bool actualDayTime = !(hour >= 18 || hour < 6);
        isDayTime = g_ManualDayNight ? g_DayTime : actualDayTime;
    }

    if (isDayTime) glClearColor(0.9f, 0.9f, 1.0f, 1.0f);
    else           glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
}


/**
 * @brief   Invoca métodos de classes específicas para
 *          inicializar a aplicação do jogo
 * @details `title`, `h` e `w` são passados para a classe
 *          `Window`, que inicializa GLAD e GLFW. Em seguida,
 *          inicializa a classe `Renderer` e a classe `Scene`.
 * 
 * @param title (const char*)
 *          nome da janela
 * @param w     (int)
 *          altura da janela
 * @param h     (int)
 *          largura da janela
 */
void Application::init(const char* title, int w, int h) {
    m_window.init(title, w, h);
    m_renderer.init(m_window.get());    // carrega shaders, texturas, modelos OBJ
    m_scene.build();                    // constrói terreno, objetos, etc.
}

/**
 * @brief   Inicia o ciclo de vida do jogo, em alto nível
 */
void Application::run() {
    m_lastTime = (float)glfwGetTime();

    while (!m_window.shouldClose()) {
        //  Controle do tempo
        float now = (float)glfwGetTime();
        float dt  = now - m_lastTime;

        //  armazena em campo pŕoprio da classe
        //  o tempo atual
        m_lastTime = now;

        //  processa um frame
        processFrame(dt);

        //  buffers e poll events
        m_window.swapAndPoll();
    }
}

/**
 * @brief   Processa um frame
 * 
 * @param dt (float)
 *          passagem do tempo
 */
void Application::processFrame(float dt) {
    //  limpa a tela, verifica se noite ou dia
    updateDayNight();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::vec3 sun = glm::normalize(glm::vec3(0.6f, 0.8f, 0.3f));
    m_skybox.draw(m_renderer.getProjectionMatrix(), m_renderer.getViewMatrix(), sun, 0.5f);  // 0.5 = meio-dia


    //  lógica do jogo
    m_scene.update(dt, m_window.get());

    //
    m_scene.resolveCollisions();                 // física/som separado da render

    m_renderer.beginFrame(m_scene.getCamera());
    m_scene.draw(m_renderer);
    m_renderer.endFrame(m_window.get());
}