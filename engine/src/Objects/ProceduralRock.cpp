#include "Objects/ProceduralRock.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <cstdlib>
#include <ctime>

ProceduralRock::ProceduralRock(glm::vec3 pos, float scale)
{
    position  = pos;
    rockScale = scale;

    Generate();
    SetupBuffers();
}

ProceduralRock::~ProceduralRock()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void ProceduralRock::Generate()
{
    generateCube();
}

void ProceduralRock::generateCube()
{
    vertices.clear();
    indices.clear();

    float s = 0.5f;

    // intensidade da deformação
    float jitter = rockScale * 0.15f;

    auto randOffset = [&]() -> float
    {
        return (
            ((float) rand() / RAND_MAX) - 0.5f
        ) * jitter;
    };

    // =====================================================
    // VÉRTICES DO CUBO DEFORMADO
    // =====================================================

    glm::vec3 p0(-s + randOffset(), -s + randOffset(), -s + randOffset());
    glm::vec3 p1( s + randOffset(), -s + randOffset(), -s + randOffset());
    glm::vec3 p2( s + randOffset(),  s + randOffset(), -s + randOffset());
    glm::vec3 p3(-s + randOffset(),  s + randOffset(), -s + randOffset());

    glm::vec3 p4(-s + randOffset(), -s + randOffset(),  s + randOffset());
    glm::vec3 p5( s + randOffset(), -s + randOffset(),  s + randOffset());
    glm::vec3 p6( s + randOffset(),  s + randOffset(),  s + randOffset());
    glm::vec3 p7(-s + randOffset(),  s + randOffset(),  s + randOffset());

    // =====================================================
    // HELPERS
    // =====================================================

    auto addTriangle =
    [&](glm::vec3 a, glm::vec3 b, glm::vec3 c)
    {
        glm::vec3 normal =
            glm::normalize(glm::cross(b - a, c - a));

        unsigned int start = vertices.size();

        vertices.push_back({
            a,
            normal,
            glm::vec2(0.0f, 0.0f)
        });

        vertices.push_back({
            b,
            normal,
            glm::vec2(1.0f, 0.0f)
        });

        vertices.push_back({
            c,
            normal,
            glm::vec2(0.5f, 1.0f)
        });

        indices.push_back(start + 0);
        indices.push_back(start + 1);
        indices.push_back(start + 2);
    };

    // =====================================================
    // 12 TRIÂNGULOS DO CUBO
    // =====================================================

    // Frente
    addTriangle(p4, p5, p6);
    addTriangle(p4, p6, p7);

    // Trás
    addTriangle(p1, p0, p3);
    addTriangle(p1, p3, p2);

    // Esquerda
    addTriangle(p0, p4, p7);
    addTriangle(p0, p7, p3);

    // Direita
    addTriangle(p5, p1, p2);
    addTriangle(p5, p2, p6);

    // Topo
    addTriangle(p3, p7, p6);
    addTriangle(p3, p6, p2);

    // Base
    addTriangle(p0, p1, p5);
    addTriangle(p0, p5, p4);
}

void ProceduralRock::SetupBuffers()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    // =====================================================
    // VBO
    // =====================================================

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        vertices.size() * sizeof(RockVertex),
        vertices.data(),
        GL_STATIC_DRAW
    );

    // =====================================================
    // EBO
    // =====================================================

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        indices.size() * sizeof(unsigned int),
        indices.data(),
        GL_STATIC_DRAW
    );

    // =====================================================
    // POSITION
    // =====================================================

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(RockVertex),
        (void*)offsetof(RockVertex, position)
    );

    glEnableVertexAttribArray(0);

    // =====================================================
    // NORMAL
    // =====================================================

    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(RockVertex),
        (void*)offsetof(RockVertex, normal)
    );

    glEnableVertexAttribArray(1);

    // =====================================================
    // TEXCOORDS
    // =====================================================

    glVertexAttribPointer(
        2,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(RockVertex),
        (void*)offsetof(RockVertex, texcoords)
    );

    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void ProceduralRock::Draw(GLuint model_uniform)
{
    model = glm::mat4(1.0f);

    model = glm::translate(
        model,
        position
    );

    model = glm::scale(
        model,
        glm::vec3(rockScale)
    );

    glUniformMatrix4fv(
        model_uniform,
        1,
        GL_FALSE,
        glm::value_ptr(model)
    );

    glBindVertexArray(VAO);

    glDrawElements(
        GL_TRIANGLES,
        indices.size(),
        GL_UNSIGNED_INT,
        0
    );

    glBindVertexArray(0);
}