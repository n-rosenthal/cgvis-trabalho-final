#pragma once

#include <GLFW/glfw3.h>

// InputHandler.hpp
// Singleton que centraliza todo o estado de entrada (teclado + mouse) e serve
// como ponto único de chamada dos callbacks GLFW.
//
// Globais removidas de main.cpp:
//   g_LeftMouseButtonPressed, g_RightMouseButtonPressed,
//   g_MiddleMouseButtonPressed, g_LastCursorPosX, g_LastCursorPosY,
//   g_AngleX, g_AngleY, g_AngleZ,
//   g_ForearmAngleX, g_ForearmAngleZ,
//   g_TorsoPositionX, g_TorsoPositionY,
//   g_ShowInfoText, g_ShowDebugPanel
//
// Lógica extraída de main.cpp:
//   KeyCallback()         → InputHandler::onKey()
//   MouseButtonCallback() → InputHandler::onMouseButton()
//   CursorPosCallback()   → InputHandler::onCursorPos()   (parte de câmera
//                           delegada a Camera; partes de bunny/antebraço aqui)
//
// USO em main.cpp:
//   // Callbacks estáticos finos que delegam ao singleton:
//   void KeyCallback(GLFWwindow* w, int key, int sc, int action, int mod) {
//       InputHandler::get().onKey(w, key, sc, action, mod);
//   }
//   void MouseButtonCallback(GLFWwindow* w, int btn, int action, int mods) {
//       InputHandler::get().onMouseButton(w, btn, action, mods);
//   }
//   void CursorPosCallback(GLFWwindow* w, double x, double y) {
//       InputHandler::get().onCursorPos(w, x, y);
//   }

// Forward declarations para evitar dependência circular
class Camera;

class InputHandler
{
public:
    // Acesso ao singleton
    static InputHandler& get();

    // Injeta referência à câmera para que os callbacks de mouse possam
    // delegar rotação de câmera a Camera::onCursorMove / onScroll.
    void setCamera(Camera* camera) { m_camera = camera; }

    // -------------------------------------------------------------------------
    // Callbacks — chamados pelos wrappers estáticos registrados no GLFW
    // -------------------------------------------------------------------------

    // Equivale a KeyCallback() de main.cpp.
    void onKey(GLFWwindow* window, int key, int scancode, int action, int mod);

    // Equivale a MouseButtonCallback() de main.cpp.
    void onMouseButton(GLFWwindow* window, int button, int action, int mods);

    // Equivale a CursorPosCallback() de main.cpp.
    // A parte de câmera (botão esquerdo) é delegada a m_camera.
    void onCursorPos(GLFWwindow* window, double xpos, double ypos);

    // Equivale a ScrollCallback() de main.cpp.
    // Delegado inteiramente a m_camera.
    void onScroll(GLFWwindow* window, double xoffset, double yoffset);

    // -------------------------------------------------------------------------
    // Estado de mouse — consultado por outros sistemas (ex: Game, UIRenderer)
    // -------------------------------------------------------------------------
    bool isLeftMouseDown()   const { return m_leftDown;   }
    bool isRightMouseDown()  const { return m_rightDown;  }
    bool isMiddleMouseDown() const { return m_middleDown; }

    double getLastCursorX() const { return m_lastCursorX; }
    double getLastCursorY() const { return m_lastCursorY; }

    // -------------------------------------------------------------------------
    // Estado legado de ângulos / posições (ainda usados pelo código de lab)
    // Serão removidos quando Game/Bird absorverem a lógica do bunny.
    // -------------------------------------------------------------------------
    float getAngleX() const { return m_angleX; }
    float getAngleY() const { return m_angleY; }
    float getAngleZ() const { return m_angleZ; }

    float getForearmAngleX() const { return m_forearmAngleX; }
    float getForearmAngleZ() const { return m_forearmAngleZ; }

    float getTorsoPositionX() const { return m_torsoPositionX; }
    float getTorsoPositionY() const { return m_torsoPositionY; }

    // Posição e rotação do bunny — serão migradas para Bird/Game
    float getBunnyPositionX() const { return m_bunnyPositionX; }
    float getBunnyPositionY() const { return m_bunnyPositionY; }
    float getBunnyPositionZ() const { return m_bunnyPositionZ; }
    float getBunnyRotationY() const { return m_bunnyRotationY; }

    // -------------------------------------------------------------------------
    // Flags de UI — consultadas por UIRenderer
    // -------------------------------------------------------------------------
    bool showInfoText()   const { return m_showInfoText;   }
    bool showDebugPanel() const { return m_showDebugPanel; }

    // Getters públicos para a posição e rotaçãoY do rubber-duck
    float getDuckPositionX() const { return m_duckPositionX; }
    float getDuckPositionY() const { return m_duckPositionY; }
    float getDuckPositionZ() const { return m_duckPositionZ; }
    float getDuckRotationY() const { return m_duckRotationY; }

private:
    InputHandler() = default;
    InputHandler(const InputHandler&) = delete;
    InputHandler& operator=(const InputHandler&) = delete;

    Camera* m_camera = nullptr;

    // Estado dos botões do mouse
    bool   m_leftDown   = false;
    bool   m_rightDown  = false;
    bool   m_middleDown = false;
    double m_lastCursorX = 0.0;
    double m_lastCursorY = 0.0;

    // Ângulos de Euler (teclas X/Y/Z do lab)
    float m_angleX = 0.0f;
    float m_angleY = 0.0f;
    float m_angleZ = 0.0f;

    // Antebraço / torso (mouse do meio)
    float m_forearmAngleX  = 0.0f;
    float m_forearmAngleZ  = 0.0f;
    float m_torsoPositionX = 0.0f;
    float m_torsoPositionY = 0.0f;

    // Bunny (mouse direito + WASD) — migrar para Bird/Game
    float m_bunnyPositionX = 0.0f;
    float m_bunnyPositionY = 0.0f;
    float m_bunnyPositionZ = 0.0f;
    float m_bunnyRotationY = 0.0f;

    // Flags de UI
    bool m_showInfoText   = true;
    bool m_showDebugPanel = true;

    // Posição e rotação do rubber-duck
    float m_duckPositionX = 0.0f;
    float m_duckPositionY = 0.0f;
    float m_duckPositionZ = 0.0f;
    float m_duckRotationY = 0.0f;

    // Setters privados para o modelo rubber-duck
    const float moveSpeed = 0.05f;
    const float rotSpeed  = 0.05f;
};