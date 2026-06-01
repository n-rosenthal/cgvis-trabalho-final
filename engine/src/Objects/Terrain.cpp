#include <glad/glad.h>

#include <cmath>
#include <stdlib.h>


#include "Objects/Terrain.hpp"
#include "Objects/TerrainRegion.hpp"
#include <cstdio>


/**
 * @brief Construidor padrão para `ProceduralTerrain`.
 * 
 * @param width (int)
 *          largura
 * @param depth (int)
 *          comprimento
 * @param spacing
 *          espaçamento
 */
Terrain::Terrain(
    int width,
    int depth,
    float spacing
) {
    m_width = width;
    m_depth = depth;
    m_spacing = spacing;
}

/**
 * @brief Destruidor padrão
 */
Terrain::~Terrain() {
    if (m_vao != 0)
        glDeleteVertexArrays(1, &m_vao);

    if (m_vbo != 0)
        glDeleteBuffers(1, &m_vbo);

    if (m_ebo != 0)
        glDeleteBuffers(1, &m_ebo);
}



/**
 * @brief Adiciona uma `TerrainLayer` ao terreno sendo construído
 * 
 * @param layer 
 *          camada de relevo (@see `HillLayer`, `LakeLayer`, `MountainLayer`...)
 */
void Terrain::addRegion(
    std::shared_ptr<TerrainRegion> layer
) {
    m_regions.push_back(layer);
}

/**
 * @brief   Limpa a coleção de camadas de relevo
 */
void Terrain::clearRegions() {
    m_regions.clear();
}


void Terrain::computeNormals()
{
    // zera normais
    for (auto& vertex : m_vertices)
    {
        vertex.normal = glm::vec3(0.0f);
    }

    // acumula normais dos triângulos
    for (size_t i = 0; i < m_indices.size(); i += 3)
    {
        GLuint ia = m_indices[i];
        GLuint ib = m_indices[i + 1];
        GLuint ic = m_indices[i + 2];

        glm::vec3 a = m_vertices[ia].position;
        glm::vec3 b = m_vertices[ib].position;
        glm::vec3 c = m_vertices[ic].position;

        glm::vec3 edge1 = b - a;
        glm::vec3 edge2 = c - a;

        glm::vec3 normal =
            glm::normalize(
                glm::cross(edge1, edge2)
            );

        m_vertices[ia].normal += normal;
        m_vertices[ib].normal += normal;
        m_vertices[ic].normal += normal;
    }

    // normaliza
    for (auto& vertex : m_vertices)
    {
        vertex.normal =
            glm::normalize(vertex.normal);
    }
}

const TerrainRegion* Terrain::getRegion(
    float x,
    float z
) const {
    TerrainRegion* region = nullptr;

    for (const auto& r : m_regions){
        if (r->contains(x, z))
        {
            region = r.get();
            return region;
        }
    }
    
    return nullptr;
}

/**
 * @brief Constrói a malha do terreno.
 *
 * Para cada vértice:
 *
 * - determina sua posição no mundo;
 * - encontra a região correspondente;
 * - calcula a altura do relevo;
 * - associa cor e material da região;
 * - inicializa normal e coordenadas de textura.
 */
void Terrain::buildMesh() {
    // =====================================================
    // LIMPEZA
    // =====================================================

    m_vertices.clear();
    m_indices.clear();

    float halfWidth =
        (m_width * m_spacing) * 0.5f;

    float halfDepth =
        (m_depth * m_spacing) * 0.5f;

    // =====================================================
    // VÉRTICES
    // =====================================================

    for (int z = 0; z <= m_depth; ++z)
    {
        for (int x = 0; x <= m_width; ++x)
        {
            float worldX =
                x * m_spacing - halfWidth;

            float worldZ =
                z * m_spacing - halfDepth;

            // =============================================
            // REGIÃO
            // =============================================

            const TerrainRegion* region = getRegion(worldX, worldZ);

            // =============================================
            // ALTURA
            // =============================================

            float worldY = 0.0f;

            if (region != nullptr)
            {
                worldY =
                    region->getHeight(
                        worldX,
                        worldZ
                    );
            }

            // =============================================
            // VÉRTICE
            // =============================================

            TerrainVertex vertex;

            vertex.position =
                glm::vec3(
                    worldX,
                    worldY,
                    worldZ
                );

            vertex.normal =
                glm::vec3(
                    0.0f,
                    1.0f,
                    0.0f
                );

            vertex.texcoord =
                glm::vec2(
                    static_cast<float>(x) / m_width,
                    static_cast<float>(z) / m_depth
                );

            // =============================================
            // COR E MATERIAL
            // =============================================

            if (region != nullptr)
            {
                vertex.color =
                    region->getColor();

                vertex.materialId =
                    region->getMaterial().id;
            }
            else
            {
                vertex.color =
                    glm::vec3(
                        1.0f,
                        0.0f,
                        1.0f
                    ); // magenta: erro

                vertex.materialId = -1;
            }

            m_vertices.push_back(vertex);


            if(region)
            {
                vertex.color = region->getColor();

                if(x % 100 == 0 && z % 100 == 0)
                {
                    printf(
                        "REGION COLOR = (%f %f %f)\n",
                        vertex.color.r,
                        vertex.color.g,
                        vertex.color.b
                    );
                }
            }
        }
    }

    // =====================================================
    // ÍNDICES
    // =====================================================

    for (int z = 0; z < m_depth; ++z)
    {
        for (int x = 0; x < m_width; ++x)
        {
            GLuint topLeft =
                z * (m_width + 1) + x;

            GLuint topRight =
                topLeft + 1;

            GLuint bottomLeft =
                (z + 1) * (m_width + 1) + x;

            GLuint bottomRight =
                bottomLeft + 1;

            // Triângulo 1

            m_indices.push_back(topLeft);
            m_indices.push_back(bottomLeft);
            m_indices.push_back(topRight);

            // Triângulo 2

            m_indices.push_back(topRight);
            m_indices.push_back(bottomLeft);
            m_indices.push_back(bottomRight);
        }
    }
}

/**
 * @brief Gera a altura do terreno em (x,z).
 */
float Terrain::generateHeight(
    float x,
    float z
) const {
    float height = 0.0f;

    for (const auto& region : m_regions) {
        if (region->contains(x, z)) {
            height += region->getHeight(x, z);
        }
    }

    return height;
}

void Terrain::setupBuffers()
{
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    glBindVertexArray(m_vao);

    // =====================================================
    // VBO
    // =====================================================

    glBindBuffer(
        GL_ARRAY_BUFFER,
        m_vbo
    );

    glBufferData(
        GL_ARRAY_BUFFER,
        m_vertices.size() * sizeof(TerrainVertex),
        m_vertices.data(),
        GL_STATIC_DRAW
    );

    // =====================================================
    // EBO
    // =====================================================

    glBindBuffer(
        GL_ELEMENT_ARRAY_BUFFER,
        m_ebo
    );

    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        m_indices.size() * sizeof(GLuint),
        m_indices.data(),
        GL_STATIC_DRAW
    );

    // =====================================================
    // POSITION (location = 0)
    // =====================================================

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(TerrainVertex),
        (void*)offsetof(
            TerrainVertex,
            position
        )
    );

    glEnableVertexAttribArray(0);

    // =====================================================
    // NORMAL (location = 1)
    // =====================================================

    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(TerrainVertex),
        (void*)offsetof(
            TerrainVertex,
            normal
        )
    );

    glEnableVertexAttribArray(1);

    // =====================================================
    // TEXCOORD (location = 2)
    // =====================================================

    glVertexAttribPointer(
        2,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(TerrainVertex),
        (void*)offsetof(
            TerrainVertex,
            texcoord
        )
    );

    glEnableVertexAttribArray(2);

    // =====================================================
    // COLOR (location = 3)
    // =====================================================

    glVertexAttribPointer(
        3,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(TerrainVertex),
        (void*)offsetof(
            TerrainVertex,
            color
        )
    );

    glEnableVertexAttribArray(3);

    // =====================================================
    // MATERIAL ID (location = 4)
    // =====================================================

    glVertexAttribIPointer(
        4,
        1,
        GL_INT,
        sizeof(TerrainVertex),
        (void*)offsetof(
            TerrainVertex,
            materialId
        )
    );

    glEnableVertexAttribArray(4);

    glBindVertexArray(0);
}

/**
 * @brief Produz a malha (mesh) do terreno
 *
 * A altura de cada vértice é obtida pela soma de todas
 * as TerrainLayers registradas.
 */
void Terrain::generate() {
    buildMesh();
    computeNormals();
    setupBuffers();
}


/**
 * @brief Desenha o terreno
 * 
 * @param model_uniform 
 */
void Terrain::draw(GLuint model_uniform) {
    glUniformMatrix4fv(
        model_uniform,
        1,
        GL_FALSE,
        glm::value_ptr(m_model)
    );

    glBindVertexArray(m_vao);

    glDrawElements(
        GL_TRIANGLES,
        (GLsizei)m_indices.size(),
        GL_UNSIGNED_INT,
        nullptr
    );

    glBindVertexArray(0);
}

/**
 * @brief Retorna a altura do terreno em (x,z).
 */
float Terrain::getHeight(
    float x,
    float z
) const
{
    for (const auto& region : m_regions)
    {
        if (region->contains(x, z))
        {
            return region->getHeight(x, z);
        }
    }

    return 0.0f;
}
/**
 * @brief Retorna a normal interpolada do terreno em (x,z).
 */
glm::vec3 Terrain::getNormal(
    float x,
    float z
) const
{
    float halfWidth =
        (m_width * m_spacing) * 0.5f;

    float halfDepth =
        (m_depth * m_spacing) * 0.5f;

    // =========================================
    // MUNDO -> GRID
    // =========================================

    float localX =
        (x + halfWidth) / m_spacing;

    float localZ =
        (z + halfDepth) / m_spacing;

    int x0 = (int)std::floor(localX);
    int z0 = (int)std::floor(localZ);

    int x1 = x0 + 1;
    int z1 = z0 + 1;

    // =========================================
    // LIMITES
    // =========================================

    if (
        x0 < 0 ||
        z0 < 0 ||
        x1 > m_width ||
        z1 > m_depth
    )
    {
        return glm::vec3(
            0.0f,
            1.0f,
            0.0f
        );
    }

    // =========================================
    // COORDENADAS BILINEARES
    // =========================================

    float tx = localX - x0;
    float tz = localZ - z0;

    int row = m_width + 1;

    int i00 = z0 * row + x0;
    int i10 = z0 * row + x1;
    int i01 = z1 * row + x0;
    int i11 = z1 * row + x1;

    glm::vec3 n00 = m_vertices[i00].normal;
    glm::vec3 n10 = m_vertices[i10].normal;
    glm::vec3 n01 = m_vertices[i01].normal;
    glm::vec3 n11 = m_vertices[i11].normal;

    // =========================================
    // INTERPOLAÇÃO BILINEAR
    // =========================================

    glm::vec3 nx0 =
        n00 * (1.0f - tx) +
        n10 * tx;

    glm::vec3 nx1 =
        n01 * (1.0f - tx) +
        n11 * tx;

    glm::vec3 normal =
        nx0 * (1.0f - tz) +
        nx1 * tz;

    return glm::normalize(normal);
}