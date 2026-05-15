// InputHandler.cpp
// Implementação de InputHandler. Veja InputHandler.hpp para documentação.
//
// Dependências:
//   - Camera.hpp  (onLeftMousePress/Release, onCursorMove, onScroll)
//   - correcao.cpp (Correcao_KeyCallback — obrigatório pelo professor)
//   - GLFW

#include "InputHandler.hpp"
#include "Camera.hpp"

#include <cstdio>  // fprintf, fflush

// Declaração externa de Correcao_KeyCallback (definida em correcao.cpp).
// Deve ser sempre o primeiro comando de onKey(), conforme instrução do prof.
void Correcao_KeyCallback(int key, int action, int mod);

// Declaração externa de LoadShadersFromFiles (ainda em main.cpp ou
// ResourceManager — atualizar quando ResourceManager for extraído).
void LoadShadersFromFiles();

// -----------------------------------------------------------------------------
// Singleton
// -----------------------------------------------------------------------------
InputHandler& InputHandler::get()
{
    static InputHandler instance;
    return instance;
}

// -----------------------------------------------------------------------------
// onKey — espelha KeyCallback() de main.cpp
// -----------------------------------------------------------------------------
void InputHandler::onKey(GLFWwindow* window, int key, int /*scancode*/,
                         int action, int mod)
{
    // Obrigatório: chamada de correção automatizada dos laboratórios.
    // NÃO remover nem mover para depois de qualquer outro código.
    Correcao_KeyCallback(key, action, mod);

    // ESC fecha a janela
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    // -------------------------------------------------------------------------
    // Ângulos de Euler (teclas X / Y / Z do laboratório)
    // -------------------------------------------------------------------------
    const float delta = 3.141592f / 16.0f; // 22.5 graus em radianos

    if (key == GLFW_KEY_X && action == GLFW_PRESS)
        m_angleX += (mod & GLFW_MOD_SHIFT) ? -delta : delta;

    if (key == GLFW_KEY_Y && action == GLFW_PRESS)
        m_angleY += (mod & GLFW_MOD_SHIFT) ? -delta : delta;

    if (key == GLFW_KEY_Z && action == GLFW_PRESS)
        m_angleZ += (mod & GLFW_MOD_SHIFT) ? -delta : delta;

    // Espaço reseta ângulos e posições legadas
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
    {
        m_angleX        = 0.0f;
        m_angleY        = 0.0f;
        m_angleZ        = 0.0f;
        m_forearmAngleX = 0.0f;
        m_forearmAngleZ = 0.0f;
        m_torsoPositionX = 0.0f;
        m_torsoPositionY = 0.0f;
    }

    // -------------------------------------------------------------------------
    // Projeção — delegado a Camera
    // -------------------------------------------------------------------------
    if (m_camera)
    {
        // P → perspectiva, O → ortográfica
        if (key == GLFW_KEY_P && action == GLFW_PRESS)
        {
            if (!m_camera->isPerspective())
                m_camera->toggleProjection();
        }
        if (key == GLFW_KEY_O && action == GLFW_PRESS)
        {
            if (m_camera->isPerspective())
                m_camera->toggleProjection();
        }
    }

    // -------------------------------------------------------------------------
    // Flags de UI
    // -------------------------------------------------------------------------
    if (key == GLFW_KEY_H && action == GLFW_PRESS)
        m_showInfoText = !m_showInfoText;

    if (key == GLFW_KEY_T && action == GLFW_PRESS)
        m_showDebugPanel = !m_showDebugPanel;

    // -------------------------------------------------------------------------
    // Recarga de shaders em tempo de execução
    // -------------------------------------------------------------------------
    if (key == GLFW_KEY_R && action == GLFW_PRESS)
    {
        LoadShadersFromFiles();
        fprintf(stdout, "Shaders recarregados!\n");
        fflush(stdout);
    }

    // -------------------------------------------------------------------------
    // Controles do bunny (WASD + setas)
    // TODO: migrar para Game::onKey() / Bird quando feature/bird for integrada.
    // -------------------------------------------------------------------------
    const float moveSpeed = 0.1f;
    const float rotSpeed  = 0.1f;

    if (key == GLFW_KEY_W && action == GLFW_PRESS) m_bunnyPositionZ -= moveSpeed;
    if (key == GLFW_KEY_S && action == GLFW_PRESS) m_bunnyPositionZ += moveSpeed;
    if (key == GLFW_KEY_A && action == GLFW_PRESS) m_bunnyPositionX -= moveSpeed;
    if (key == GLFW_KEY_D && action == GLFW_PRESS) m_bunnyPositionX += moveSpeed;
    if (key == GLFW_KEY_Q && action == GLFW_PRESS) m_bunnyPositionY += moveSpeed;
    if (key == GLFW_KEY_E && action == GLFW_PRESS) m_bunnyPositionY -= moveSpeed;

    // NOTA: havia um bug no main.cpp original — o bloco de GLFW_KEY_LEFT
    // estava sem o "if", causando incremento incondicional de BunnyRotationY
    // a cada KeyCallback. O bug foi corrigido aqui.
    if (key == GLFW_KEY_LEFT  && action == GLFW_PRESS) m_bunnyRotationY += rotSpeed;
    if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) m_bunnyRotationY -= rotSpeed;
}

// -----------------------------------------------------------------------------
// onMouseButton — espelha MouseButtonCallback() de main.cpp
// -----------------------------------------------------------------------------
void InputHandler::onMouseButton(GLFWwindow* window, int button,
                                 int action, int /*mods*/)
{
    // Botão esquerdo — controla rotação da câmera
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)
        {
            double x, y;
            glfwGetCursorPos(window, &x, &y);
            m_lastCursorX = x;
            m_lastCursorY = y;
            m_leftDown = true;

            if (m_camera)
                m_camera->onLeftMousePress(x, y);
        }
        else if (action == GLFW_RELEASE)
        {
            m_leftDown = false;
            if (m_camera)
                m_camera->onLeftMouseRelease();
        }
    }

    // Botão direito — controla posição/rotação do bunny (legado → Bird)
    if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        if (action == GLFW_PRESS)
        {
            glfwGetCursorPos(window, &m_lastCursorX, &m_lastCursorY);
            m_rightDown = true;
        }
        else if (action == GLFW_RELEASE)
        {
            m_rightDown = false;
        }
    }

    // Botão do meio — controla posição do torso (legado)
    if (button == GLFW_MOUSE_BUTTON_MIDDLE)
    {
        if (action == GLFW_PRESS)
        {
            glfwGetCursorPos(window, &m_lastCursorX, &m_lastCursorY);
            m_middleDown = true;
        }
        else if (action == GLFW_RELEASE)
        {
            m_middleDown = false;
        }
    }
}

// -----------------------------------------------------------------------------
// onCursorPos — espelha CursorPosCallback() de main.cpp
// -----------------------------------------------------------------------------
void InputHandler::onCursorPos(GLFWwindow* /*window*/, double xpos, double ypos)
{
    // Botão esquerdo → câmera (delegado a Camera)
    if (m_leftDown && m_camera)
        m_camera->onCursorMove(xpos, ypos);

    // Botão direito → bunny (legado — migrar para Bird)
    if (m_rightDown)
    {
        float dx = static_cast<float>(xpos - m_lastCursorX);
        float dy = static_cast<float>(ypos - m_lastCursorY);
        m_bunnyRotationY -= 0.01f * dx;
        m_bunnyPositionY += 0.01f * dy;
        m_lastCursorX = xpos;
        m_lastCursorY = ypos;
    }

    // Botão do meio → torso (legado)
    if (m_middleDown)
    {
        float dx = static_cast<float>(xpos - m_lastCursorX);
        float dy = static_cast<float>(ypos - m_lastCursorY);
        m_torsoPositionX += 0.01f * dx;
        m_torsoPositionY -= 0.01f * dy;
        m_lastCursorX = xpos;
        m_lastCursorY = ypos;
    }
}

// -----------------------------------------------------------------------------
// onScroll — espelha ScrollCallback() de main.cpp, delegado a Camera
// -----------------------------------------------------------------------------
void InputHandler::onScroll(GLFWwindow* /*window*/,
                            double /*xoffset*/, double yoffset)
{
    if (m_camera)
        m_camera->onScroll(yoffset);
}