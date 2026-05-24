#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

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

    ~ProceduralRock();

    void Draw(GLuint model_uniform);

private:

    // =====================================================
    // Geração da geometria
    // =====================================================

    void Generate();
    void generateCube();

    // =====================================================
    // OpenGL
    // =====================================================

    void SetupBuffers();

private:

    GLuint VAO = 0;
    GLuint VBO = 0;
    GLuint EBO = 0;

    glm::vec3 position;
    float rockScale;

    glm::mat4 model = glm::mat4(1.0f);

    std::vector<RockVertex> vertices;
    std::vector<unsigned int> indices;
};