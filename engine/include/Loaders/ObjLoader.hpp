#pragma once
/**
 * @file    ObjLoader.hpp
 * @brief   Loader de modelos geométricos no formato ".obj"
 */
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <tiny_obj_loader.h>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <limits>
#include <cassert>
#include <cstdio>
#include <stdexcept>

// =====================================================
// ESTRUTURAS
// =====================================================

struct ObjModel {
    tinyobj::attrib_t                attrib;
    std::vector<tinyobj::shape_t>    shapes;
    std::vector<tinyobj::material_t> materials;

    ObjModel(const char* filename, const char* basepath = NULL, bool triangulate = true);
};

struct SceneObject {
    std::string name;
    size_t      first_index;
    size_t      num_indices;
    GLenum      rendering_mode;
    GLuint      vertex_array_object_id;
    glm::vec3   bbox_min;
    glm::vec3   bbox_max;
};

// =====================================================
// CENA VIRTUAL
// =====================================================

extern std::map<std::string, SceneObject> g_VirtualScene;

// =====================================================
// FUNÇÕES
// =====================================================

void BuildTrianglesAndAddToVirtualScene(ObjModel* model);
void ComputeNormals(ObjModel* model);
void PrintObjModelInfo(ObjModel*); // Função para debugging
void DrawVirtualObject(const char* name);