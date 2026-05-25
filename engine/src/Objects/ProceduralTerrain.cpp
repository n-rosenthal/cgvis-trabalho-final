#include "Objects/ProceduralTerrain.hpp"

#include <cmath>

ProceduralTerrain::ProceduralTerrain(
    int width,
    int depth,
    float spacing,
    float amplitude,
    float frequency
)
{
    m_width = width;
    m_depth = depth;

    m_spacing = spacing;

    m_amplitude = amplitude;
    m_frequency = frequency;
}

ProceduralTerrain::~ProceduralTerrain()
{
    if (m_vao != 0)
        glDeleteVertexArrays(1, &m_vao);

    if (m_vbo != 0)
        glDeleteBuffers(1, &m_vbo);

    if (m_ebo != 0)
        glDeleteBuffers(1, &m_ebo);
}

float ProceduralTerrain::generateHeight(float x, float z) const
{
    return
        m_amplitude *
        std::sin(x * m_frequency) *
        std::cos(z * m_frequency);
}


void ProceduralTerrain::generate()
{
    buildMesh();
    computeNormals();
    setupBuffers();
}

void ProceduralTerrain::buildMesh()
{
    m_vertices.clear();
    m_indices.clear();

    float halfWidth = (m_width * m_spacing) * 0.5f;
    float halfDepth = (m_depth * m_spacing) * 0.5f;

    for (int z = 0; z <= m_depth; ++z)
    {
        for (int x = 0; x <= m_width; ++x)
        {
            float worldX = x * m_spacing - halfWidth;
            float worldZ = z * m_spacing - halfDepth;

            float y = generateHeight(worldX, worldZ);

            TerrainVertex vertex;

            vertex.position = glm::vec3(worldX, y, worldZ);

            vertex.normal = glm::vec3(0.0f, 1.0f, 0.0f);

            vertex.texcoord = glm::vec2(
                (float)x / (float)m_width,
                (float)z / (float)m_depth
            );

            m_vertices.push_back(vertex);
        }
    }

    for (int z = 0; z < m_depth; ++z)
    {
        for (int x = 0; x < m_width; ++x)
        {
            int topLeft =
                z * (m_width + 1) + x;

            int topRight =
                topLeft + 1;

            int bottomLeft =
                (z + 1) * (m_width + 1) + x;

            int bottomRight =
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

void ProceduralTerrain::computeNormals()
{
    for (auto& vertex : m_vertices)
    {
        vertex.normal = glm::vec3(0.0f);
    }

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
            glm::normalize(glm::cross(edge1, edge2));

        m_vertices[ia].normal += normal;
        m_vertices[ib].normal += normal;
        m_vertices[ic].normal += normal;
    }

    for (auto& vertex : m_vertices)
    {
        vertex.normal =
            glm::normalize(vertex.normal);
    }
}

void ProceduralTerrain::setupBuffers()
{
    glGenVertexArrays(1, &m_vao);

    glGenBuffers(1, &m_vbo);

    glGenBuffers(1, &m_ebo);

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    glBufferData(
        GL_ARRAY_BUFFER,
        m_vertices.size() * sizeof(TerrainVertex),
        m_vertices.data(),
        GL_STATIC_DRAW
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);

    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        m_indices.size() * sizeof(GLuint),
        m_indices.data(),
        GL_STATIC_DRAW
    );

    // Position
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(TerrainVertex),
        (void*)offsetof(TerrainVertex, position)
    );

    glEnableVertexAttribArray(0);

    // Normal
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(TerrainVertex),
        (void*)offsetof(TerrainVertex, normal)
    );

    glEnableVertexAttribArray(1);

    // Texcoord
    glVertexAttribPointer(
        2,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(TerrainVertex),
        (void*)offsetof(TerrainVertex, texcoord)
    );

    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void ProceduralTerrain::draw(GLuint model_uniform)
{
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
        0
    );

    glBindVertexArray(0);
}

float ProceduralTerrain::getHeight(float x, float z) const {
    float halfWidth =
        (m_width * m_spacing) * 0.5f;

    float halfDepth =
        (m_depth * m_spacing) * 0.5f;

    // converte mundo -> grid local
    float localX =
        (x + halfWidth) / m_spacing;

    float localZ =
        (z + halfDepth) / m_spacing;

    int x0 = (int)floor(localX);
    int z0 = (int)floor(localZ);

    int x1 = x0 + 1;
    int z1 = z0 + 1;

    // limites
    if (
        x0 < 0 || z0 < 0 ||
        x1 > m_width ||
        z1 > m_depth
    )
    {
        return 0.0f;
    }

    float tx = localX - x0;
    float tz = localZ - z0;

    int row = m_width + 1;

    int i00 = z0 * row + x0;
    int i10 = z0 * row + x1;
    int i01 = z1 * row + x0;
    int i11 = z1 * row + x1;

    float h00 = m_vertices[i00].position.y;
    float h10 = m_vertices[i10].position.y;
    float h01 = m_vertices[i01].position.y;
    float h11 = m_vertices[i11].position.y;

    // interpolação bilinear
    float hx0 =
        h00 * (1.0f - tx) + h10 * tx;

    float hx1 =
        h01 * (1.0f - tx) + h11 * tx;

    return
        hx0 * (1.0f - tz)
        + hx1 * tz;
}

glm::vec3 ProceduralTerrain::getNormal(float x, float z) const
{
    float eps = 0.1f;

    float hL = getHeight(x - eps, z);
    float hR = getHeight(x + eps, z);

    float hD = getHeight(x, z - eps);
    float hU = getHeight(x, z + eps);

    glm::vec3 normal =
        glm::normalize(glm::vec3(
            hL - hR,
            2.0f,
            hD - hU
        ));

    return normal;
}