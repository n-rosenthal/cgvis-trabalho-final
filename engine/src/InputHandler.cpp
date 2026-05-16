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
#include <cmath>   // sinf, cosf


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
    // Projeção — delegado a Camera
    // -------------------------------------------------------------------------
    if (m_camera)
    {
        if (key == GLFW_KEY_P && action == GLFW_PRESS && !m_camera->isPerspective())
            m_camera->toggleProjection();
        if (key == GLFW_KEY_O && action == GLFW_PRESS && m_camera->isPerspective())
            m_camera->toggleProjection();
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

    // ── Movimento do rubber-duck ──────────────────────────────────────────────
    // W/S: move na direção que o pato aponta (frente/trás no espaço local)
    // A/D: rotaciona o pato em torno do eixo Y
    // Q/E: sobe/desce
    if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        m_duckPositionX -= moveSpeed * sinf(m_duckRotationY);
        m_duckPositionZ -= moveSpeed * cosf(m_duckRotationY);
    }
    if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        m_duckPositionX += moveSpeed * sinf(m_duckRotationY);
        m_duckPositionZ += moveSpeed * cosf(m_duckRotationY);
    }
    if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT))
        m_duckRotationY += rotSpeed;
    if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT))
        m_duckRotationY -= rotSpeed;
    if (key == GLFW_KEY_Q && (action == GLFW_PRESS || action == GLFW_REPEAT))
        m_duckPositionY += moveSpeed;
    if (key == GLFW_KEY_E && (action == GLFW_PRESS || action == GLFW_REPEAT))
        m_duckPositionY -= moveSpeed;
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
}

// -----------------------------------------------------------------------------
// onCursorPos — espelha CursorPosCallback() de main.cpp
// -----------------------------------------------------------------------------
void InputHandler::onCursorPos(GLFWwindow* /*window*/, double xpos, double ypos)
{
    // Botão esquerdo → câmera (delegado a Camera).
    // Atualizamos lastCursor APÓS a chamada para que Camera::onCursorMove
    // possa calcular o delta em relação à posição anterior.
    if (m_leftDown && m_camera)
    {
        m_camera->onCursorMove(xpos, ypos);
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