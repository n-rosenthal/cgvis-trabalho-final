/**
 *  @file WindowCallbacks.cpp
 *  @brief Definições de callbacks para GLFW
 */

#include <GLFW/glfw3.h>
#include <stdio.h>

extern float g_ScreenRatio;

// Definimos o callback para impressão de erros da GLFW no terminal
void ErrorCallback(int error, const char* description);


// Definição da função que será chamada sempre que a janela do sistema
// operacional for redimensionada, por consequência alterando o tamanho do
// "framebuffer" (região de memória onde são armazenados os pixels da imagem).
void FramebufferSizeCallback(GLFWwindow* window, int width, int height);