#include "Objects/ProceduralRock.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>

#include <cmath>
#include <cstdlib>

// ============================================================
// RUÍDO
// ============================================================

static float noise(float x, float y, float z)
{
    return
        sin(x * 8.0f) *
        cos(y * 7.0f) *
        sin(z * 9.0f);
}

static float fbm(glm::vec3 p)
{
    constexpr int octaves = 5;

    float value = 0.0f;
    float amplitude = 1.0f;
    float frequency = 1.0f;

    for (int i = 0; i < octaves; i++)
    {
        value += amplitude *
            noise(
                p.x * frequency,
                p.y * frequency,
                p.z * frequency
            );

        frequency *= 2.0f;
        amplitude *= 0.5f;
    }

    return value;
}

// ============================================================
// CONSTRUTOR
// ============================================================

ProceduralRock::ProceduralRock(
    glm::vec3 pos,
    float s,
    int subdivisions
)
{
    position = pos;
    scale = s;

    generateSphere(subdivisions);

    computeNormals();

    setupBuffers();

    collisionRadius = scale * 0.95f;

    model = glm::mat4(1.0f);
}

ProceduralRock::~ProceduralRock()
{
    if (VAO)
        glDeleteVertexArrays(1, &VAO);

    if (VBO)
        glDeleteBuffers(1, &VBO);

    if (EBO)
        glDeleteBuffers(1, &EBO);
}

// ============================================================
// NORMAIS
// ============================================================

void ProceduralRock::computeNormals()
{
    for (auto& v : vertices)
        v.normal = glm::vec3(0.0f);

    for (size_t i = 0; i < indices.size(); i += 3)
    {
        RockVertex& a = vertices[indices[i]];
        RockVertex& b = vertices[indices[i + 1]];
        RockVertex& c = vertices[indices[i + 2]];

        glm::vec3 edge1 =
            b.position - a.position;

        glm::vec3 edge2 =
            c.position - a.position;

        glm::vec3 n =
            glm::normalize(
                glm::cross(edge1, edge2)
            );

        a.normal += n;
        b.normal += n;
        c.normal += n;
    }

    for (auto& v : vertices)
    {
        v.normal =
            glm::normalize(v.normal);
    }
}

// ============================================================
// ESFERA PROCEDURAL
// ============================================================

void ProceduralRock::generateSphere(int subdivisions)
{
    vertices.clear();
    indices.clear();

    int stacks =
        8 * (subdivisions + 1);

    int slices =
        16 * (subdivisions + 1);

    for (int stack = 0; stack <= stacks; ++stack)
    {
        float v =
            (float)stack /
            (float)stacks;

        float phi =
            v * glm::pi<float>();

        for (int slice = 0; slice <= slices; ++slice)
        {
            float u =
                (float)slice /
                (float)slices;

            float theta =
                u * glm::two_pi<float>();

            glm::vec3 p;

            p.x = sin(phi) * cos(theta);
            p.y = cos(phi);
            p.z = sin(phi) * sin(theta);

            // ==================================
            // DEFORMAÇÃO PROCEDURAL
            // ==================================

            float displacement =
                glm::smoothstep(
                    -1.0f,
                    1.0f,
                    fbm(p * 0.8f)
                ) * 2.0f - 1.0f;

            float radius =
                1.0f +
                displacement * 0.08f;

            p *= radius;

            RockVertex vertex;

            vertex.position = p;
            vertex.normal =
                glm::normalize(p);

            vertex.texcoords =
                glm::vec2(u, v);

            vertices.push_back(vertex);
        }
    }

    for (int stack = 0; stack < stacks; ++stack)
    {
        for (int slice = 0; slice < slices; ++slice)
        {
            int first =
                stack * (slices + 1) +
                slice;

            int second =
                first +
                slices +
                1;

            indices.push_back(first);
            indices.push_back(first + 1);
            indices.push_back(second);

            indices.push_back(second);
            indices.push_back(first + 1);
            indices.push_back(second + 1);
        }
    }
}

// ============================================================
// COLISÃO
// ============================================================

bool ProceduralRock::checkCollision(
    glm::vec3 point,
    float radius
)
{
    float distance =
        glm::length(point - position);

    bool collision =
        distance <
        (collisionRadius + radius);

    if(collision && !collided)
    {
        collided = true;
        return true;
    }

    if(!collision)
    {
        collided = false;
    }

    return false;
}

// ============================================================
// OPENGL
// ============================================================

void ProceduralRock::setupBuffers()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        vertices.size() *
        sizeof(RockVertex),
        vertices.data(),
        GL_STATIC_DRAW
    );

    glBindBuffer(
        GL_ELEMENT_ARRAY_BUFFER,
        EBO
    );

    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        indices.size() *
        sizeof(unsigned int),
        indices.data(),
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(RockVertex),
        (void*)offsetof(
            RockVertex,
            position
        )
    );
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(RockVertex),
        (void*)offsetof(
            RockVertex,
            normal
        )
    );
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(
        2,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(RockVertex),
        (void*)offsetof(
            RockVertex,
            texcoords
        )
    );
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

// ============================================================
// RENDER
// ============================================================

void ProceduralRock::Draw(
    GLuint model_uniform
)
{
    glm::mat4 model =
        glm::mat4(1.0f);

    model = glm::translate(
        model,
        position
    );

    model = glm::scale(
        model,
        glm::vec3(scale)
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
        static_cast<GLsizei>(
            indices.size()
        ),
        GL_UNSIGNED_INT,
        nullptr
    );

    glBindVertexArray(0);
}