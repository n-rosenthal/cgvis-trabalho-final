#pragma once

#include <vector>

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

struct TerrainVertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texcoord;
};

class ProceduralTerrain
{
public:

    ProceduralTerrain(
        int width,
        int depth,
        float spacing,
        float amplitude,
        float frequency
    );

    ~ProceduralTerrain();

    void generate();
    void draw(GLuint model_uniform);

    float heightAt(float x, float z) const;

private:

    int m_width;
    int m_depth;

    float m_spacing;

    float m_amplitude;
    float m_frequency;

    GLuint m_vao = 0;
    GLuint m_vbo = 0;
    GLuint m_ebo = 0;

    std::vector<TerrainVertex> m_vertices;
    std::vector<GLuint> m_indices;

    glm::mat4 m_model = glm::mat4(1.0f);

private:

    float generateHeight(float x, float z) const;

    void buildMesh();
    void computeNormals();
    void setupBuffers();
};