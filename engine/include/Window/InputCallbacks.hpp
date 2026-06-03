/**
 * @brief Definições de callbacks para GLFW
 */

#pragma once

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "matrices.h"

extern bool     g_ShowInfoText;
extern bool     g_ShowDebugPanel;
extern bool     g_UsePerspectiveProjection;
extern bool     g_ManualDayNight;
extern bool     g_DayTime;
extern float    g_AngleX;
extern float    g_AngleY;
extern float    g_AngleZ;

extern double&    g_LastCursorPosX;
extern double&    g_LastCursorPosY;
extern bool&     g_LeftMouseButtonPressed;
extern bool&     g_RightMouseButtonPressed;
extern bool&     g_MiddleMouseButtonPressed;

extern float g_CameraTheta;
extern float g_CameraPhi;
extern float g_CameraDistance;

/**
 * @brief Definições de callbacks para GLFW
 * 
 * @param key 
 * @param action 
 * @param mod 
 */

void Correcao_KeyCallback(int key, int action, int mod);

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mod);

void TextRendering_ShowModelViewProjection(
    GLFWwindow* window,
    glm::mat4 projection,
    glm::mat4 view,
    glm::mat4 model,
    glm::vec4 p_model
);

void TextRendering_ShowEulerAngles(GLFWwindow* window);

void TextRendering_ShowProjection(GLFWwindow* window);

void TextRendering_ShowFramesPerSecond(GLFWwindow* window);

void TextRendering_ShowDebugPanel(GLFWwindow* window);

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

void MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

