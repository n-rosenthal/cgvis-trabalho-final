/**
 * @file    RingDrawable.cpp
 */
#include "Objects/Drawables/RingDrawable.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

RingDrawable::RingDrawable(float radius) : m_radius(radius) {}

// buildMesh
void RingDrawable::buildMesh() {
    // Layout de cada vértice: position(3) + normal(3) + texcoord(2) = 8 floats
    // Armazenamos em m_vertices (herdado de Drawable) como Vertex
    m_vertices.clear();
    m_indices.clear();

    const int   segments    = 128;
    const float outerRadius = m_radius;
    const float innerRadius = m_radius * 0.55f;

    for (int i = 0; i <= segments; ++i) {
        float theta = (float)i / segments * 2.0f * (float)M_PI;
        float c = cos(theta), s = sin(theta);

        Vertex outer, inner;
        outer.position = glm::vec4(c * outerRadius, s * outerRadius, 0.0f, 1.0f);
        outer.normal   = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
        outer.texcoord = glm::vec2(1.0f, 0.0f);
        outer.color    = glm::vec4(1.0f);

        inner.position = glm::vec4(c * innerRadius, s * innerRadius, 0.0f, 1.0f);
        inner.normal   = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
        inner.texcoord = glm::vec2(0.0f, 1.0f);
        inner.color    = glm::vec4(1.0f);

        m_vertices.push_back(outer);
        m_vertices.push_back(inner);
    }

    for (int i = 0; i < segments; ++i) {
        int start = i * 2;
        m_indices.push_back(start);
        m_indices.push_back(start + 1);
        m_indices.push_back(start + 2);
        m_indices.push_back(start + 1);
        m_indices.push_back(start + 3);
        m_indices.push_back(start + 2);
    }

    m_indexCount = (int)m_indices.size();
}

// setupBuffers

void RingDrawable::setupBuffers() {
    glBindVertexArray(m_buffers.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_buffers.VBO);
    glBufferData(GL_ARRAY_BUFFER,
                 m_vertices.size() * sizeof(Vertex),
                 m_vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 m_indices.size() * sizeof(GLuint),
                 m_indices.data(), GL_STATIC_DRAW);

    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);
    // normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);
    // texcoord
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, texcoord));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

// draw

void RingDrawable::draw(const DrawContext& ctx) {
    // Pulsação
    float pulse = 1.0f + sin(m_pulseTime * 4.0f) * 0.08f;

    // Billboard: usa posição do modelo mas orienta para a câmera
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, m_position);

    glm::mat4 billboard = glm::inverse(m_view);
    billboard[3] = glm::vec4(0, 0, 0, 1);   // remove translação
    model *= billboard;

    model = glm::scale(model, glm::vec3(m_animScale * pulse));

    glUniformMatrix4fv(ctx.model_uniform, 1, GL_FALSE, glm::value_ptr(model));
    glUniform1i(ctx.object_id_uniform, ctx.object_id);

    glBindVertexArray(m_buffers.VAO);
    glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}