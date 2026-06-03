
#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <cstdio>
#include <vector>
#include "stb_image.h"

extern GLuint g_NumLoadedTextures;

// Função que carrega uma imagem para ser utilizada como textura
void LoadTextureImage(const char* filename);