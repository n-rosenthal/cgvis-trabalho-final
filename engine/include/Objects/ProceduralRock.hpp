#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>

#include <vector>



struct RockVertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texcoords;
};

class ProceduralRock
{
public:

    ProceduralRock(glm::vec3 pos, float scale);

    void Draw();

private:

    void Generate();
    void SetupBuffers();

    GLuint VAO;
    GLuint VBO;
    GLuint EBO;

    glm::vec3 position;
    float rockScale;

    std::vector<RockVertex> vertices;
    std::vector<unsigned int> indices;
};