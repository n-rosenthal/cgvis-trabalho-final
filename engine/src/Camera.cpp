// Camera.cpp
// Implementação de Camera. Veja Camera.hpp para a documentação completa.
//
// Dependências:
//   - matrices.h  (Matrix_Camera_View, Matrix_Perspective, Matrix_Orthographic)
//   - glm         (vec4, mat4, value_ptr)

#include "Camera.hpp"
#include "matrices.h"

#include <algorithm> // std::clamp
#include <limits>

// -----------------------------------------------------------------------------
// Construtor — espelha os valores padrão das globais do main.cpp original
// -----------------------------------------------------------------------------
Camera::Camera()
    : m_theta(0.0f)
    , m_phi(0.0f)
    , m_distance(3.5f)
    , m_lookAtX(0.0f), m_lookAtY(0.0f), m_lookAtZ(0.0f)
    , m_leftButtonDown(false)
    , m_lastCursorX(0.0), m_lastCursorY(0.0)
    , m_screenRatio(1.0f)
    , m_usePerspective(true)
{}

// -----------------------------------------------------------------------------
// Look-at
// -----------------------------------------------------------------------------
void Camera::setLookAt(float x, float y, float z)
{
    m_lookAtX = x;
    m_lookAtY = y;
    m_lookAtZ = z;
}

// -----------------------------------------------------------------------------
// Callbacks de mouse
// -----------------------------------------------------------------------------

// Equivale ao bloco "if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)"
// que estava em MouseButtonCallback() em main.cpp.
void Camera::onLeftMousePress(double cursorX, double cursorY)
{
    m_leftButtonDown = true;
    m_lastCursorX    = cursorX;
    m_lastCursorY    = cursorY;
}

// Equivale ao bloco "if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)".
void Camera::onLeftMouseRelease()
{
    m_leftButtonDown = false;
}

// Equivale ao bloco "if (g_LeftMouseButtonPressed)" dentro de CursorPosCallback().
void Camera::onCursorMove(double xpos, double ypos)
{
    if (!m_leftButtonDown)
        return;

    float dx = static_cast<float>(xpos - m_lastCursorX);
    float dy = static_cast<float>(ypos - m_lastCursorY);

    m_theta -= 0.01f * dx;
    m_phi   += 0.01f * dy;

    // Phi deve ficar entre -pi/2 e +pi/2 para evitar gimbal lock.
    m_phi = std::clamp(m_phi, -PHI_MAX, PHI_MAX);

    m_lastCursorX = xpos;
    m_lastCursorY = ypos;
}

// Equivale a ScrollCallback() em main.cpp.
void Camera::onScroll(double yoffset)
{
    m_distance -= 0.1f * static_cast<float>(yoffset);

    // A câmera look-at não pode ficar exatamente sobre o ponto alvo
    // (divisão por zero na construção do sistema de coordenadas).
    const float verysmallnumber = std::numeric_limits<float>::epsilon();
    if (m_distance < verysmallnumber)
        m_distance = verysmallnumber;
}

// -----------------------------------------------------------------------------
// Posição cartesiana (coordenadas esféricas → cartesianas)
// -----------------------------------------------------------------------------
// Equivale às linhas:
//   float r = g_CameraDistance;
//   float y = r*sin(g_CameraPhi);
//   float z = r*cos(g_CameraPhi)*cos(g_CameraTheta);
//   float x = r*cos(g_CameraPhi)*sin(g_CameraTheta);
// que estavam no loop de renderização de main.cpp.
glm::vec4 Camera::getPosition() const
{
    float x = m_distance * std::cos(m_phi) * std::sin(m_theta);
    float y = m_distance * std::sin(m_phi);
    float z = m_distance * std::cos(m_phi) * std::cos(m_theta);
    return glm::vec4(x + m_lookAtX, y + m_lookAtY, z + m_lookAtZ, 1.0f);
}

// -----------------------------------------------------------------------------
// Matriz View
// -----------------------------------------------------------------------------
// Equivale ao bloco:
//   glm::vec4 camera_position_c  = glm::vec4(x,y,z,1.0f);
//   glm::vec4 camera_lookat_l    = glm::vec4(0,0,0,1.0f);
//   glm::vec4 camera_view_vector = camera_lookat_l - camera_position_c;
//   glm::vec4 camera_up_vector   = glm::vec4(0,1,0,0);
//   glm::mat4 view = Matrix_Camera_View(...);
// que estava no loop de renderização de main.cpp.
glm::mat4 Camera::getViewMatrix() const
{
    glm::vec4 pos    = getPosition();
    glm::vec4 lookAt = glm::vec4(m_lookAtX, m_lookAtY, m_lookAtZ, 1.0f);
    glm::vec4 view   = lookAt - pos;                       // vetor "view"
    glm::vec4 up     = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f); // "céu"

    return Matrix_Camera_View(pos, view, up);
}

// -----------------------------------------------------------------------------
// Matriz Projection
// -----------------------------------------------------------------------------
// Equivale ao bloco if/else (perspectiva vs. ortográfica) do loop de main.cpp.
glm::mat4 Camera::getProjectionMatrix() const
{
    if (m_usePerspective)
    {
        return Matrix_Perspective(FOV, m_screenRatio, NEAR_PLANE, FAR_PLANE);
    }
    else
    {
        // Zoom ortográfico simulado com g_CameraDistance, exatamente como
        // estava em main.cpp.
        float t = 1.5f * m_distance / 2.5f;
        float b = -t;
        float r =  t * m_screenRatio;
        float l = -r;
        return Matrix_Orthographic(l, r, b, t, NEAR_PLANE, FAR_PLANE);
    }
}