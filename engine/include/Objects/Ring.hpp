#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

class Ring {
public:
    Ring(glm::vec3 pos, float radius);

    void update(float dt);
    void draw(GLuint model_uniform);
    bool checkCollision(glm::vec3 birdPos);
    bool isDead() const;

private:
    void setupMesh();

    GLuint VAO;
    GLuint VBO;
    GLuint EBO;

    glm::vec3 position;

    float radius;
    float pulseTime;
    bool collected;
    float destroyTimer;
    float scale;
    int indexCount;
};