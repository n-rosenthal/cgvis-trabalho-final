#include "Objects/Ring.hpp"

#include <GLFW/glfw3.h>
#include <vector>
#include <cmath>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Ring::Ring(glm::vec3 pos, float r) {
    position = pos;
    radius = r;
    pulseTime = 0.0f;
    collected = false;
    destroyTimer = 0.0f;
    scale = 1.0f;

    VAO = 0;
    VBO = 0;
    EBO = 0;

    indexCount = 0;

    setupMesh();
}

void Ring::update(float dt)
{
    pulseTime += dt;
    
    if (collected)
    {
        destroyTimer += dt;

        scale += dt * 3.0f;
    }
}

bool Ring::checkCollision(glm::vec3 birdPos)
{
    if (collected)
        return false;

    glm::vec3 delta = birdPos - position;

    float horizontalDistance =
        sqrt(delta.x * delta.x + delta.z * delta.z);

    bool insideRing =
        horizontalDistance < radius &&
        fabs(delta.y) < 2.0f;

    if (insideRing)
    {
        collected = true;

        // TODO:if (collected)
        // tocar som aqui
        // playSound("ring.wav");

        return true;
    }

    return false;
}

bool Ring::isDead() const
{
    return destroyTimer > 1.0f;
}

void Ring::draw(
    GLuint model_uniform,
    const glm::mat4& view
){
    //  Pulsação do anel
    float pulse = 1.0f + sin(pulseTime * 4.0f) * 0.08f;

    //  Matriz de transformação do anel
    glm::mat4 model = glm::mat4(1.0f);

    //  Translação do anel à posição correta
    model = glm::translate(model, position);

    // billboard
    glm::mat4 billboard = glm::inverse(view);

    // remove translação
    billboard[3] = glm::vec4(0,0,0,1);

    // aplica billboard
    model *= billboard;

    //  Escala do anel
    model *= glm::scale(
        glm::mat4(1.0f),
        glm::vec3(scale * pulse)
    );

    //  Envia a matriz de modelagem para o shader
    glUniformMatrix4fv(
        model_uniform,
        1,
        GL_FALSE,
        glm::value_ptr(model)
    );

    //  Desenha o anel
    glBindVertexArray(VAO);
    glDrawElements(
        GL_TRIANGLES,
        indexCount,
        GL_UNSIGNED_INT,
        0
    );
    glBindVertexArray(0);
}

void Ring::setupMesh()
{
    //  Geração da geometria
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    //  Quantidade de segmentos
    const int segments = 128;

    //  Raio externo e interno
    float outerRadius = radius;
    float innerRadius = radius * 0.55f;

    for (int i = 0; i <= segments; i++) {
        float theta =
            (float)i / (float)segments
            * 2.0f
            * M_PI;

        float c = cos(theta);
        float s = sin(theta);

        // =========================
        // VÉRTICE EXTERNO
        // =========================

        vertices.push_back(c * outerRadius);
        vertices.push_back(s * outerRadius);
        vertices.push_back(0.0f);

        vertices.push_back(0.0f);
        vertices.push_back(0.0f);
        vertices.push_back(1.0f);

        vertices.push_back(1.0f);
        vertices.push_back(0.0f);

        // =========================
        // VÉRTICE INTERNO
        // =========================

        vertices.push_back(c * innerRadius);
        vertices.push_back(s * innerRadius);
        vertices.push_back(0.0f);

        vertices.push_back(0.0f);
        vertices.push_back(0.0f);
        vertices.push_back(1.0f);

        vertices.push_back(0.0f);
        vertices.push_back(1.0f);
    }

    for (int i = 0; i < segments; i++) {
        int start = i * 2;

        indices.push_back(start);
        indices.push_back(start + 1);
        indices.push_back(start + 2);

        indices.push_back(start + 1);
        indices.push_back(start + 3);
        indices.push_back(start + 2);
    }

    indexCount = (int)indices.size();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        vertices.size() * sizeof(float),
        vertices.data(),
        GL_STATIC_DRAW
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        indices.size() * sizeof(unsigned int),
        indices.data(),
        GL_STATIC_DRAW
    );

    // position
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        8 * sizeof(float),
        (void*)0
    );

    glEnableVertexAttribArray(0);

    // normal
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        8 * sizeof(float),
        (void*)(3 * sizeof(float))
    );

    glEnableVertexAttribArray(1);

    // texcoords
    glVertexAttribPointer(
        2,
        2,
        GL_FLOAT,
        GL_FALSE,
        8 * sizeof(float),
        (void*)(6 * sizeof(float))
    );

    glEnableVertexAttribArray(2);
    glBindVertexArray(0);
}