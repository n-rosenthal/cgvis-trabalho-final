#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

struct BuildingVertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texcoords;
};

class Building
{
public:

    Building(
        glm::vec3 position,
        float width,
        float height,
        float depth
    );

    ~Building();

    void Draw(GLuint model_uniform);

    // ============================================
    // Colisão
    // ============================================

    bool checkCollision(
        glm::vec3 point,
        float radius
    );

    // ============================================
    // Getters
    // ============================================

    glm::vec3 getPosition() const
    {
        return position;
    }

    glm::vec3 getMin() const
    {
        return position - dimensions * 0.5f;
    }

    glm::vec3 getMax() const
    {
        return position + dimensions * 0.5f;
    }

private:

    void generateBox();
    void setupBuffers();

private:

    GLuint VAO = 0;
    GLuint VBO = 0;
    GLuint EBO = 0;

    glm::vec3 position;
    glm::vec3 dimensions;

    glm::mat4 model = glm::mat4(1.0f);

    bool collided = false;

    std::vector<BuildingVertex> vertices;
    std::vector<unsigned int> indices;
};