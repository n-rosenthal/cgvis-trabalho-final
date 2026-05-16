#ifndef VIRTUAL_MODEL_H
#define VIRTUAL_MODEL_H

#include <string>
#include <glm/glm.hpp>
#include <glad/glad.h>

// Estrutura que armazena dados de um modelo carregado de arquivo OBJ
// (VAO, índices, bounding box). Não confundir com a classe SceneObject de Objects.h.
struct VirtualModel
{
    std::string  name;           // Nome do objeto (geralmente o nome da shape no OBJ)
    size_t       first_index;    // Primeiro índice no vetor global de índices
    size_t       num_indices;    // Número de índices
    GLenum       rendering_mode; // Modo de rasterização (GL_TRIANGLES)
    GLuint       vertex_array_object_id; // ID do VAO
    glm::vec3    bbox_min;       // Bounding box mínimo
    glm::vec3    bbox_max;       // Bounding box máximo
};

#endif