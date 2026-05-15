#pragma once

#include <cmath>
#include <limits>

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

// Camera.hpp
// Encapsula os parâmetros esféricos da câmera look-at e calcula as matrizes
// View e Projection. Substitui as globais:
//   g_CameraTheta, g_CameraPhi, g_CameraDistance, g_ScreenRatio,
//   g_UsePerspectiveProjection
// e os trechos de CursorPosCallback(), ScrollCallback() e do loop principal
// que computavam as matrizes view/projection.

class Camera
{
public:
    // Constrói a câmera com os mesmos valores padrão do main.cpp original.
    Camera();

    // -------------------------------------------------------------------------
    // Parâmetros esféricos
    // -------------------------------------------------------------------------
    float getTheta()    const { return m_theta;    }
    float getPhi()      const { return m_phi;       }
    float getDistance() const { return m_distance;  }

    // Ponto fixo para onde a câmera olha (origem por padrão).
    // Futuramente pode ser ajustado para seguir a ave:
    //   camera.setLookAt(g_Bird.getPosition());
    void setLookAt(float x, float y, float z);

    // -------------------------------------------------------------------------
    // Callbacks de mouse — chamados pelo InputHandler (ou direto do GLFW)
    // -------------------------------------------------------------------------

    // Deve ser chamado quando o botão esquerdo é pressionado para registrar
    // o ponto de âncora do cursor.
    void onLeftMousePress(double cursorX, double cursorY);

    // Deve ser chamado quando o botão esquerdo é solto.
    void onLeftMouseRelease();

    // Deve ser chamado a cada CursorPosCallback enquanto o botão esq. estiver
    // pressionado. Atualiza theta e phi conforme o deslocamento do cursor.
    void onCursorMove(double xpos, double ypos);

    // Deve ser chamado a cada ScrollCallback. Atualiza a distância (zoom).
    void onScroll(double yoffset);

    // -------------------------------------------------------------------------
    // Razão de aspecto — atualizada pelo FramebufferSizeCallback
    // -------------------------------------------------------------------------
    void setScreenRatio(float ratio) { m_screenRatio = ratio; }
    float getScreenRatio() const     { return m_screenRatio;  }

    // -------------------------------------------------------------------------
    // Tipo de projeção
    // -------------------------------------------------------------------------
    void toggleProjection() { m_usePerspective = !m_usePerspective; }
    bool isPerspective() const { return m_usePerspective; }

    // -------------------------------------------------------------------------
    // Matrizes prontas para enviar à GPU
    // -------------------------------------------------------------------------

    // Retorna a matriz View calculada a partir das coordenadas esféricas atuais.
    glm::mat4 getViewMatrix() const;

    // Retorna a matriz Projection (perspectiva ou ortográfica) conforme o modo
    // atual e a razão de aspecto da janela.
    glm::mat4 getProjectionMatrix() const;

    // Posição cartesiana da câmera no espaço (útil para debug/UIRenderer).
    glm::vec4 getPosition() const;

private:
    // Coordenadas esféricas
    float m_theta;     // Ângulo no plano ZX em relação ao eixo Z
    float m_phi;       // Ângulo de elevação em relação ao eixo Y
    float m_distance;  // Distância ao ponto look-at

    // Ponto para onde a câmera olha
    float m_lookAtX, m_lookAtY, m_lookAtZ;

    // Estado do mouse
    bool   m_leftButtonDown;
    double m_lastCursorX, m_lastCursorY;

    // Janela / projeção
    float m_screenRatio;
    bool  m_usePerspective;

    // Constantes de projeção
    static constexpr float NEAR_PLANE   = -0.1f;
    static constexpr float FAR_PLANE    = -10.0f;
    static constexpr float FOV          = 3.141592f / 3.0f; // 60°
    static constexpr float PHI_MAX      = 3.141592f / 2.0f;
};