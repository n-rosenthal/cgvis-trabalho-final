// ProceduralRock.hpp
#pragma once
#include <glad/glad.h>

#include <vector>
#include <random>


#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../matrices.h"

struct RockVertex
{
    glm::vec3 position;
    glm::vec3 normal;
};

struct RockInstance
{
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    int biome_id;
};

class ProceduralRock
{
public:
    ProceduralRock();

    // Gera o mesh base da rocha
    void generateBaseMesh(
        int subdivisions = 2,
        float radius = 1.0f,
        float noise_strength = 0.35f
    );

    // Espalha rochas pelo terreno
    void generateInstances(
        int count,
        float terrain_size,
        float (*heightFunc)(float, float),
        int (*biomeFunc)(float, float) = nullptr
    );

    // Renderização
    void draw(
        GLuint model_uniform,
        GLuint object_id_uniform,
        int object_id
    );

    // Configurações
    void setSeed(unsigned int seed);

    // Limpeza
    ~ProceduralRock();

private:
    // Mesh
    std::vector<RockVertex> vertices;
    std::vector<GLuint> indices;

    GLuint VAO;
    GLuint VBO;
    GLuint EBO;

    // Rochas espalhadas
    std::vector<RockInstance> instances;

    // RNG
    std::mt19937 rng;

private:
    void createIcosahedron();
    void subdivide();

    void computeNormals();

    float randomFloat(float a, float b);

    glm::vec3 randomRotation();
};