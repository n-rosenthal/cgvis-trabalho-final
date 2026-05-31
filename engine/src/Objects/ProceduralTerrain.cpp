#include "Objects/ProceduralTerrain.hpp"
#include <cmath>


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
ProceduralTerrain::ProceduralTerrain(
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
ProceduralTerrain::~ProceduralTerrain() {
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
void ProceduralTerrain::addLayer(
    std::shared_ptr<TerrainLayer> layer
) {
    m_layers.push_back(layer);
}

/**
 * @brief   Limpa a coleção de camadas de relevo
 */
void ProceduralTerrain::clearLayers() {
    m_layers.clear();
}


void ProceduralTerrain::computeNormals()
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


/**
 * @brief   Constrói a malha (mesh) do terreno, de acordo com
 *          a coleção de camadas de relevo.
*/ 
void ProceduralTerrain::buildMesh() {
    //  limpe a geometria atual do terreno
    m_vertices.clear();
    m_indices.clear();

    //  largura e profundidade do terreno
    float halfWidth = (m_width * m_spacing) * 0.5f;
    float halfDepth = (m_depth * m_spacing) * 0.5f;

    // =====================================================
    // VÉRTICES
    // =====================================================

    //  para cada ponto (x, z), faça...
    for (int z = 0; z <= m_depth; ++z) {
        for (int x = 0; x <= m_width; ++x) {
            //  obtenha as coordenadas do mundo
            float worldX = x * m_spacing - halfWidth;
            float worldZ = z * m_spacing - halfDepth;

            //  em função das coordenadas do mundo
            //  compute a altura y = h(x, z) do terreno
            float worldY = generateHeight(worldX, worldZ);

            //  inicialize um vértice de terreno
            TerrainVertex vertex;

            //  compute sua posição, vetor normal e coordenadas de textura
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
                    (float)x / m_width,
                    (float)z / m_depth
                );

            //  adiciona o vértice ao mesh
            m_vertices.push_back(vertex);
        }
    }

    // =====================================================
    // ÍNDICES
    // =====================================================

    for (int z = 0; z < m_depth; ++z) {
        for (int x = 0; x < m_width; ++x) {
            unsigned int topLeft        = z * (m_width + 1) + x;
            unsigned int topRight       = topLeft + 1;
            unsigned int bottomLeft     = (z + 1) * (m_width + 1) + x;

            unsigned int bottomRight    = bottomLeft + 1;

            // triângulo 1
            m_indices.push_back(
                topLeft
            );

            m_indices.push_back(
                bottomLeft
            );

            m_indices.push_back(
                topRight
            );

            // triângulo 2

            m_indices.push_back(
                topRight
            );

            m_indices.push_back(
                bottomLeft
            );

            m_indices.push_back(
                bottomRight
            );
        }
    }
}

/**
 * @brief   "Gera" a altura do relevo de acordo com a camada de relevo
 *          no ponto (x, z). Retorna a altura y = h(x, z)
 * 
 * @param x 
 * @param z 
 * @return float 
 */
float ProceduralTerrain::generateHeight(
    float x,
    float z
) const {
    //  Inicializa altura (y = h(x, z))
    float height = 0.0f;

    //  para cada camada de relevo, invoque a função sample(x, z)
    //  que produz a altura y do relevo, e some à altura do terreno
    for (const auto& layer : m_layers) {
        height +=
            layer->sample(x, z);
    }

    return height;
}

void ProceduralTerrain::setupBuffers()
{
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    glBindVertexArray(m_vao);

    // ====================================
    // VBO
    // ====================================

    glBindBuffer(
        GL_ARRAY_BUFFER,
        m_vbo
    );

    glBufferData(
        GL_ARRAY_BUFFER,
        m_vertices.size() *
            sizeof(TerrainVertex),
        m_vertices.data(),
        GL_STATIC_DRAW
    );

    // ====================================
    // EBO
    // ====================================

    glBindBuffer(
        GL_ELEMENT_ARRAY_BUFFER,
        m_ebo
    );

    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        m_indices.size() *
            sizeof(GLuint),
        m_indices.data(),
        GL_STATIC_DRAW
    );

    // ====================================
    // POSITION
    // ====================================

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

    // ====================================
    // NORMAL
    // ====================================

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

    // ====================================
    // TEXCOORD
    // ====================================

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

    glBindVertexArray(0);
}

/**
 * @brief Produz a malha (mesh) do terreno
 *
 * A altura de cada vértice é obtida pela soma de todas
 * as TerrainLayers registradas.
 */
void ProceduralTerrain::generate() {
    buildMesh();
    computeNormals();
    setupBuffers();
}


/**
 * @brief Desenha o terreno
 * 
 * @param model_uniform 
 */
void ProceduralTerrain::draw(GLuint model_uniform) {
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
 * @brief   Obtenha a altura do relevo no ponto (x, z)
 * 
 * @param x 
 * @param z 
 * @return float 
 */
float ProceduralTerrain::getHeight(float x, float z) const {
    //  centro do terreno
    float halfWidth = (m_width * m_spacing) * 0.5f;
    float halfDepth = (m_depth * m_spacing) * 0.5f;

    // mundo -> grid
    float localX    = (x + halfWidth) / m_spacing;
    float localZ    = (z + halfDepth) / m_spacing;
    int x0          = (int)floor(localX);
    int z0          = (int)floor(localZ);

    int x1 = x0 + 1;
    int z1 = z0 + 1;

    // fora do terreno
    if (x0 < 0 ||
        z0 < 0 ||
        x1 > m_width ||
        z1 > m_depth) {
        return 0.0f;
    }

    float tx    = localX - x0;
    float tz    = localZ - z0;
    int row     = m_width + 1;
    int i00     = z0 * row + x0;
    int i10     = z0 * row + x1;
    int i01     = z1 * row + x0;
    int i11     = z1 * row + x1;
    float h00   = m_vertices[i00].position.y;
    float h10   = m_vertices[i10].position.y;
    float h01   = m_vertices[i01].position.y;
    float h11   = m_vertices[i11].position.y;
    float hx0   = h00 * (1.0f - tx) + h10 * tx;
    float hx1   = h01 * (1.0f - tx) + h11 * tx;

    return        hx0 * (1.0f - tz) + hx1 * tz;
}

/**
 * @brief   Obtenha a normal do relevo no ponto (x, z)
 * 
 * @param x 
 * @param z 
 * @return glm::vec3 
 */
glm::vec3 ProceduralTerrain::getNormal(float x, float z) const {
    float halfWidth = (m_width * m_spacing) * 0.5f;
    float halfDepth = (m_depth * m_spacing) * 0.5f;

    // =========================================
    // MUNDO -> GRID
    // =========================================

    float localX    = (x + halfWidth) / m_spacing;
    float localZ    = (z + halfDepth) / m_spacing;
    int x0          = (int)floor(localX);
    int z0          = (int)floor(localZ);
    int x1          = x0 + 1;
    int z1          = z0 + 1;

    // =========================================
    // LIMITES
    // =========================================
    if (
        x0 < 0 ||
        z0 < 0 ||
        x1 > m_width ||
        z1 > m_depth) {
        return glm::vec3(
            0.0f,
            1.0f,
            0.0f
        );
    }

    // =========================================
    // INTERPOLAÇÃO
    // =========================================

    float tx    = localX - x0;
    float tz    = localZ - z0;
    int row     = m_width + 1;
    int i00     = z0 * row + x0;
    int i10     = z0 * row + x1;
    int i01     = z1 * row + x0;
    int i11     = z1 * row + x1;

    glm::vec3 n00 = m_vertices[i00].normal;
    glm::vec3 n10 = m_vertices[i10].normal;
    glm::vec3 n01 = m_vertices[i01].normal;
    glm::vec3 n11 = m_vertices[i11].normal;

    // =========================================
    // INTERPOLAÇÃO BILINEAR
    // =========================================
    glm::vec3 nx0    = n00 * (1.0f - tx) + n10 * tx;
    glm::vec3 nx1    = n01 * (1.0f - tx) + n11 * tx;
    glm::vec3 normal = nx0 * (1.0f - tz) + nx1 * tz;
    
    return glm::normalize(normal);
}