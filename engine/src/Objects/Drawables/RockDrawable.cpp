/**
 * @file    RockDrawable.cpp
 * @brief   Implementação do RockDrawable.
 *          A geometria procedural (esfera deformada por fBm) é
 *          idêntica à do ProceduralRock original — só foi movida
 *          para cá para respeitar a interface Drawable.
 */
#include "Objects/Drawables/RockDrawable.hpp"
#include <glm/gtc/constants.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>

// Ruído

float RockDrawable::noise(float x, float y, float z) {
    return sin(x * 8.0f) * cos(y * 7.0f) * sin(z * 9.0f);
}

float RockDrawable::fbm(glm::vec3 p) {
    float value = 0.0f, amplitude = 1.0f, frequency = 1.0f;
    for (int i = 0; i < 5; ++i) {
        value     += amplitude * noise(p.x * frequency, p.y * frequency, p.z * frequency);
        frequency *= 2.0f;
        amplitude *= 0.5f;
    }
    return value;
}

// Construtor

RockDrawable::RockDrawable(int subdivisions)
    : m_subdivisions(subdivisions) {}

// buildMesh

void RockDrawable::buildMesh() {
    m_rockVerts.clear();
    m_rockIdx.clear();

    const int stacks = 8  * (m_subdivisions + 1);
    const int slices = 16 * (m_subdivisions + 1);

    for (int stack = 0; stack <= stacks; ++stack) {
        float v   = (float)stack / stacks;
        float phi = v * glm::pi<float>();

        for (int slice = 0; slice <= slices; ++slice) {
            float u     = (float)slice / slices;
            float theta = u * glm::two_pi<float>();

            glm::vec3 p {
                sin(phi) * cos(theta),
                cos(phi),
                sin(phi) * sin(theta)
            };

            float disp   = glm::smoothstep(-1.0f, 1.0f, fbm(p * 0.8f)) * 2.0f - 1.0f;
            float radius = 1.0f + disp * 0.08f;
            p *= radius;

            m_rockVerts.push_back({ p, glm::normalize(p), glm::vec2(u, v) });
        }
    }

    for (int stack = 0; stack < stacks; ++stack) {
        for (int slice = 0; slice < slices; ++slice) {
            int first  = stack * (slices + 1) + slice;
            int second = first + slices + 1;

            m_rockIdx.push_back(first);
            m_rockIdx.push_back(first + 1);
            m_rockIdx.push_back(second);

            m_rockIdx.push_back(second);
            m_rockIdx.push_back(first + 1);
            m_rockIdx.push_back(second + 1);
        }
    }
}

// computeNormals

void RockDrawable::computeNormals() {
    for (auto& v : m_rockVerts) v.normal = glm::vec3(0.0f);

    for (size_t i = 0; i < m_rockIdx.size(); i += 3) {
        auto& a = m_rockVerts[m_rockIdx[i]];
        auto& b = m_rockVerts[m_rockIdx[i + 1]];
        auto& c = m_rockVerts[m_rockIdx[i + 2]];
        glm::vec3 n = glm::normalize(glm::cross(b.position - a.position,
                                                 c.position - a.position));
        a.normal += n;
        b.normal += n;
        c.normal += n;
    }

    for (auto& v : m_rockVerts) v.normal = glm::normalize(v.normal);
}

// setupBuffers

void RockDrawable::setupBuffers() {
    glBindVertexArray(m_buffers.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_buffers.VBO);
    glBufferData(GL_ARRAY_BUFFER,
                 m_rockVerts.size() * sizeof(RockVertex),
                 m_rockVerts.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 m_rockIdx.size() * sizeof(GLuint),
                 m_rockIdx.data(), GL_STATIC_DRAW);

    // location 0 — position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(RockVertex),
                          (void*)offsetof(RockVertex, position));
    glEnableVertexAttribArray(0);

    // location 1 — normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(RockVertex),
                          (void*)offsetof(RockVertex, normal));
    glEnableVertexAttribArray(1);

    // location 2 — texcoords
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(RockVertex),
                          (void*)offsetof(RockVertex, texcoords));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

// draw 
void RockDrawable::draw(const DrawContext& ctx) {
    glBindVertexArray(m_buffers.VAO);
    glDrawElements(GL_TRIANGLES,
                   static_cast<GLsizei>(m_rockIdx.size()),
                   GL_UNSIGNED_INT, nullptr);

    glUniform1i(
        glGetUniformLocation(
            ctx.shader_program,
            "useTexture"
        ),
        GL_FALSE
    );
    
    glBindVertexArray(0);
}