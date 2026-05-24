#include "Objects/ProceduralRock.hpp"

#include <glm/gtc/constants.hpp>
#include <cstdlib>
#include <ctime>

static float randf(float a, float b)
{
    return a + (b-a)*(float(rand())/float(RAND_MAX));
}

ProceduralRock::ProceduralRock(glm::vec3 pos, float scale)
{
    position = pos;
    rockScale = scale;

    Generate();
    SetupBuffers();
}

void ProceduralRock::Generate()
{
    vertices.clear();
    indices.clear();

    // ============================================================
    // Base: cubo low-poly
    // ============================================================

    std::vector<glm::vec3> base =
    {
        {-1,-1,-1},
        { 1,-1,-1},
        { 1, 1,-1},
        {-1, 1,-1},

        {-1,-1, 1},
        { 1,-1, 1},
        { 1, 1, 1},
        {-1, 1, 1},
    };

    // deformação irregular
    for (auto& v : base)
    {
        float noise = randf(0.7f, 1.4f);

        v.x *= noise * randf(0.8f, 1.2f);
        v.y *= noise * randf(0.7f, 1.3f);
        v.z *= noise * randf(0.8f, 1.2f);

        // achata levemente
        v.y *= 0.7f;
    }

    // ============================================================
    // Faces trianguladas
    // ============================================================

    int tris[] =
    {
        0,1,2,  2,3,0,
        4,5,6,  6,7,4,
        0,4,7,  7,3,0,
        1,5,6,  6,2,1,
        3,2,6,  6,7,3,
        0,1,5,  5,4,0
    };

    // ============================================================
    // Flat shading:
    // duplicamos vértices por triângulo
    // ============================================================

    for (int i = 0; i < 36; i += 3)
    {
        glm::vec3 a = base[tris[i]];
        glm::vec3 b = base[tris[i+1]];
        glm::vec3 c = base[tris[i+2]];

        glm::vec3 normal =
            glm::normalize(glm::cross(b-a, c-a));

        RockVertex va;
        va.position = a * rockScale + position;
        va.normal = normal;
        va.texcoords = glm::vec2(0,0);

        RockVertex vb;
        vb.position = b * rockScale + position;
        vb.normal = normal;
        vb.texcoords = glm::vec2(0,0);

        RockVertex vc;
        vc.position = c * rockScale + position;
        vc.normal = normal;
        vc.texcoords = glm::vec2(0,0);

        vertices.push_back(va);
        vertices.push_back(vb);
        vertices.push_back(vc);

        indices.push_back(vertices.size()-3);
        indices.push_back(vertices.size()-2);
        indices.push_back(vertices.size()-1);
    }
}

void ProceduralRock::SetupBuffers()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        vertices.size()*sizeof(RockVertex),
        vertices.data(),
        GL_STATIC_DRAW
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        indices.size()*sizeof(unsigned int),
        indices.data(),
        GL_STATIC_DRAW
    );

    // posição
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(RockVertex),
        (void*)0
    );
    glEnableVertexAttribArray(0);

    // normal
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(RockVertex),
        (void*)offsetof(RockVertex, normal)
    );
    glEnableVertexAttribArray(1);

    // texcoord
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

void ProceduralRock::Draw()
{
    glBindVertexArray(VAO);

    glDrawElements(
        GL_TRIANGLES,
        indices.size(),
        GL_UNSIGNED_INT,
        0
    );

    glBindVertexArray(0);
}