#include "Objects/Building.hpp"

#include <glm/gtc/matrix_transform.hpp>

Building::Building(
    glm::vec3 pos,
    float width,
    float height,
    float depth
)
{
    position = pos;

    dimensions = glm::vec3(
        width,
        height,
        depth
    );

    generateBox();
    setupBuffers();

    model =
        glm::translate(
            glm::mat4(1.0f),
            position
        );
}

Building::~Building()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Building::generateBox()
{
    vertices.clear();
    indices.clear();

    float hx = dimensions.x * 0.5f;
    float hy = dimensions.y * 0.5f;
    float hz = dimensions.z * 0.5f;

    glm::vec3 p[8] =
    {
        {-hx,-hy,-hz},
        { hx,-hy,-hz},
        { hx, hy,-hz},
        {-hx, hy,-hz},

        {-hx,-hy, hz},
        { hx,-hy, hz},
        { hx, hy, hz},
        {-hx, hy, hz}
    };

    for (int i = 0; i < 8; ++i)
    {
        BuildingVertex v;

        v.position = p[i];
        v.normal =
            glm::normalize(p[i]);

        v.texcoords =
            glm::vec2(0.0f);

        vertices.push_back(v);
    }

    unsigned int idx[] =
    {
        // frente
        4,5,6,
        4,6,7,

        // trás
        0,2,1,
        0,3,2,

        // esquerda
        0,4,7,
        0,7,3,

        // direita
        1,2,6,
        1,6,5,

        // topo
        3,7,6,
        3,6,2,

        // base
        0,1,5,
        0,5,4
    };

    indices.assign(
        idx,
        idx + sizeof(idx)/sizeof(unsigned int)
    );
}

void Building::setupBuffers()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        vertices.size() *
        sizeof(BuildingVertex),
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
        sizeof(BuildingVertex),
        (void*)offsetof(
            BuildingVertex,
            position
        )
    );

    glEnableVertexAttribArray(0);

    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(BuildingVertex),
        (void*)offsetof(
            BuildingVertex,
            normal
        )
    );

    glEnableVertexAttribArray(1);

    glVertexAttribPointer(
        2,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(BuildingVertex),
        (void*)offsetof(
            BuildingVertex,
            texcoords
        )
    );

    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void Building::Draw(
    GLuint model_uniform
)
{
    glUniformMatrix4fv(
        model_uniform,
        1,
        GL_FALSE,
        glm::value_ptr(model)
    );

    glBindVertexArray(VAO);

    glDrawElements(
        GL_TRIANGLES,
        (GLsizei)indices.size(),
        GL_UNSIGNED_INT,
        0
    );

    glBindVertexArray(0);
}

bool Building::checkCollision(
    glm::vec3 point,
    float radius
)
{
    glm::vec3 minBox = getMin();
    glm::vec3 maxBox = getMax();

    glm::vec3 closest =
        glm::clamp(
            point,
            minBox,
            maxBox
        );

    float distance =
        glm::length(
            point - closest
        );

    bool isColliding =
        distance < radius;

    if (isColliding && !collided)
    {
        collided = true;
        return true;
    }

    if (!isColliding)
    {
        collided = false;
    }

    return false;
}