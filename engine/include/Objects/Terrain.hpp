#pragma once

/**
 * @file    Terrain.hpp
 * @brief   `Terrain` é um AGREGADOR de CAMADAS DE RELEVO (`TerrainLayer`).
 * 
 * @details `Terrain` é responsável por (1.) acumular em uma coleção as
 *          as camadas de relevo distintas para as áreas do jogo, e
 *          (2.) servir de interface para funcionalidades do jogo e interação de 
 *          outros objetos com o relevo.
 */

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <memory>
#include <vector>

#include "Objects/TerrainLayer.hpp"
#include "Objects/TerrainRegion.hpp"

/**
 * @brief Um vértice do terreno é uma 3-upla
 *  (posição, vetor normal, coordenadas de textura)
 *  
 *  @param position (glm::vec3)
 *          posição local do vértice no cubo
 *  @param normal (glm::vec3)
 *          vetor normal do vértice
 *  @param texcoords (glm::vec2)
 *          coordenadas de textura do vértice
 * 
 *  @param color (glm::vec3)
 *          cor do vértice
 *  @param materialId (int)
 *          identificador do material do vértice
 */
struct TerrainVertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texcoord;
    glm::vec3 color;
    int materialId;
};

class Terrain{
public:

    // =====================================================
    // CONSTRUTOR / DESTRUTOR
    // =====================================================

    Terrain(
        int width,
        int depth,
        float spacing
    );

    ~Terrain();

    // =====================================================
    // Regiões de terreno
    // =====================================================

    void addRegion(
        std::shared_ptr<TerrainRegion>
    );

    void clearRegions();

    // =====================================================
    // GERAÇÃO
    // =====================================================

    void generate();

    // =====================================================
    // RENDER
    // =====================================================

    void draw(GLuint model_uniform);

    // =====================================================
    // Acessadores
    // =====================================================

    float getHeight(float x, float z) const;
    glm::vec3 getNormal(float x, float z) const;

private:

    // =====================================================
    // DIMENSÕES
    // =====================================================

    int m_width;
    int m_depth;

    float m_spacing;

    // =====================================================
    // REGIÕES de TERRENO
    // =====================================================

    std::vector<
        std::shared_ptr<TerrainRegion>
    > m_regions;

    const TerrainRegion* getRegion(float x, float z) const;

    // =====================================================
    // OPENGL
    // =====================================================

    GLuint m_vao = 0;
    GLuint m_vbo = 0;
    GLuint m_ebo = 0;

    // =====================================================
    // GEOMETRIA
    // =====================================================

    std::vector<TerrainVertex> m_vertices;
    std::vector<GLuint> m_indices;

    glm::mat4 m_model =
        glm::mat4(1.0f);

private:
    float generateHeight(
        float x,
        float z
    ) const;

    void buildMesh();

    void computeNormals();

    void setupBuffers();
};