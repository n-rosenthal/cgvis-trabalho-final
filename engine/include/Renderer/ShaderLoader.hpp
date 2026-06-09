#pragma once

/**
 * @file    `ShaderLoader.hpp`
 * @brief   Loader para shaders
 */

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <stdexcept>

// Variáveis globais do programa de GPU
extern GLuint g_GpuProgramID;
extern GLint  g_model_uniform;
extern GLint  g_view_uniform;
extern GLint  g_projection_uniform;
extern GLint  g_object_id_uniform;
extern GLint  g_bbox_min_uniform;
extern GLint  g_bbox_max_uniform;
extern GLint  g_diffuse_texture_uniform;


// Carrega os shaders de vértices e de fragmentos
void LoadShadersFromFiles();

// Carrega um Vertex Shader de um arquivo GLSL.
GLuint LoadShader_Vertex(const char* filename);

// Carrega um Fragment Shader de um arquivo GLSL.
GLuint LoadShader_Fragment(const char* filename);

// Função auxilar, utilizada pelas duas funções acima. Carrega código de GPU de
// um arquivo GLSL e faz sua compilação.
void LoadShader(const char* filename, GLuint shader_id);

// Esta função cria um programa de GPU, o qual contém obrigatoriamente um
// Vertex Shader e um Fragment Shader.
GLuint CreateGpuProgram(GLuint vertex_shader_id, GLuint fragment_shader_id);