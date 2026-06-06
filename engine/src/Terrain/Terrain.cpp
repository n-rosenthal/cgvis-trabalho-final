/**
 * @file    `Terrain.cpp`
 * @brief   Implementação de `Terrain`
 */

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/gl.h>

#include <cmath>
#include <cstdio>
#include <stdlib.h>


#include "Terrain/Terrain.hpp"

//  onda sinusoidal
//  y(t) = A * sin(ωt + φ) = A * sin(2π/T * t + φ)

//  onda cosinusoidal
//  y(t) = A * cos(ωt + φ) = A * cos(2π/T * t + φ)

auto swave(float A, float omega, float phi, float t) -> float
{
    return A * std::sin((glm::two_pi<float>() / omega) * t + phi);
}

auto cwave(float A, float omega, float phi, float t) -> float
{
    return A * std::cos((glm::two_pi<float>() / omega) * t + phi);
}

float testWave(float x, float z) {
    return 0.5f * swave(1.0f, 4.0f, 0.0f, x) + 0.5f * cwave(1.0f, 2.0f, 0.0f, z);
}



//  Funções de altura y = h(x, z)
float longWaves(float x, float z) {
    return  (1.0f / 3.0f)                       //  período
            *   std::sin((3.0f / 4.0f) * x)     //  fase
            *   std::cos((1.0f / 2.0f) * z);    //  amplitude
}


//  Funções de coloração c = f(h)
glm::vec3 greenish(float y) {
    float t = (y + 0.5f) * 4.0f;

    return glm::vec3(
        0.2f + 0.8f * t,
        0.7f * t,
        0.2f + 0.8f * t
    );
}

glm::vec3 grassColor(float y, float minY, float maxY) {
    // Normalize and clamp y to [0,1]
    float t = (y - minY) / (maxY - minY);
    t = glm::clamp(t, 0.0f, 1.0f);

    // Define low and high green colors
    glm::vec3 darkGreen(0.1f, 0.5f, 0.1f);   // RGB
    glm::vec3 lightGreen(0.4f, 0.9f, 0.2f);  // yellowish green

    // Linear interpolation
    return darkGreen * (1.0f - t) + lightGreen * t;
}


Terrain::Terrain(
    int width,
    int depth,
    float spacing
)
:
    m_width(width),
    m_depth(depth),
    m_spacing(spacing)
{
}

void Terrain::buildMesh()
{
    m_vertices.clear();
    m_indices.clear();

    float halfWidth =
        m_width * m_spacing * 0.5f;

    float halfDepth =
        m_depth * m_spacing * 0.5f;

    for(int z=0; z<=m_depth; ++z)
    {
        for(int x=0; x<=m_width; ++x)
        {
            float wx =
                x*m_spacing-halfWidth;

            float wz =
                z*m_spacing-halfDepth;

            float wy =
                sampleHeight(wx,wz);

            Vertex v;

            v.position =
            {
                wx,
                wy,
                wz
            };

            v.normal =
            {
                0,
                1,
                0
            };

            v.texcoord =
            {
                (float)x/m_width,
                (float)z/m_depth
            };

            v.color =
                sampleColor(wy);

            m_vertices.push_back(v);
        }
    }

    for(int z=0; z<m_depth; ++z)
    {
        for(int x=0; x<m_width; ++x)
        {
            GLuint tl =
                z*(m_width+1)+x;

            GLuint tr =
                tl+1;

            GLuint bl =
                (z+1)*(m_width+1)+x;

            GLuint br =
                bl+1;

            m_indices.push_back(tl);
            m_indices.push_back(bl);
            m_indices.push_back(tr);

            m_indices.push_back(tr);
            m_indices.push_back(bl);
            m_indices.push_back(br);
        }
    }
}

void Terrain::computeNormals()
{
    for(auto& v : m_vertices)
        v.normal = glm::vec3(0,1,0);
}

void Terrain::setupBuffers()
{
    glBindVertexArray(m_buffers.VAO);

    glBindBuffer(
        GL_ARRAY_BUFFER,
        m_buffers.VBO
    );

    glBufferData(
        GL_ARRAY_BUFFER,
        m_vertices.size()*sizeof(Vertex),
        m_vertices.data(),
        GL_STATIC_DRAW
    );

    glBindBuffer(
        GL_ELEMENT_ARRAY_BUFFER,
        m_buffers.EBO
    );

    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        m_indices.size()*sizeof(GLuint),
        m_indices.data(),
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        (void*)offsetof(Vertex,position)
    );
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        (void*)offsetof(Vertex,normal)
    );
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(
        2,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        (void*)offsetof(Vertex,texcoord)
    );
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(
        3,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        (void*)offsetof(Vertex,color)
    );
    glEnableVertexAttribArray(3);

    glBindVertexArray(0);
}

void Terrain::draw(
    const DrawContext&
)
{
    glBindVertexArray(
        m_buffers.VAO
    );

    glDrawElements(
        GL_TRIANGLES,
        m_indices.size(),
        GL_UNSIGNED_INT,
        nullptr
    );

    glBindVertexArray(0);
}


float Terrain::sampleHeight(
    float x,
    float z
) const
{
    return longWaves(x, z);
}

glm::vec3 Terrain::sampleColor(
    float y
) const
{
    return grassColor(y, -3.0f, 3.0f);
}

float Terrain::getHeight(
    float x,
    float z
) const
{
    return sampleHeight(x,z);
}

glm::vec3 Terrain::getNormal(
    float x,
    float z
) const
{
    //  Aproximação: normal é sempre (0,1,0)
    return glm::vec3(0,1,0);
}



