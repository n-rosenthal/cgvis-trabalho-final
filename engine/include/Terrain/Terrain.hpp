#pragma once

#include "Objects/Interfaces/Drawable.hpp"
#include <glm/glm.hpp>
#include <vector>

class Terrain : public Drawable
{
public:
    Terrain(int width, int depth, float spacing);

    float     getHeight     (float x, float z) const;
    glm::vec3 getNormal     (float x, float z) const;
    glm::vec3 getIslandCenter(int index)        const;

    static constexpr float WATER_LEVEL = -10.2f;

    // Island descriptor (world-space units after scaling)
    struct IslandDef {
        float angle, radius, height, size;
        bool  house;
    };

protected:
    void buildMesh()      override;
    void computeNormals() override;
    void setupBuffers()   override;
    void draw(const DrawContext& ctx) override;

private:
    int   m_width, m_depth;
    float m_spacing;
    float m_halfW, m_halfD;   // world-space half-extents (precomputed)

    std::vector<Vertex>    m_vertices;
    std::vector<GLuint>    m_indices;
    struct { GLuint VAO, VBO, EBO; } m_buffers = {0,0,0};

    // Island table lazily built in world units from fraction constants
    mutable std::vector<IslandDef> m_islandCache;
    const std::vector<IslandDef>& islandTable() const;

    // Height layers
    float sampleHeight    (float x, float z) const;
    float borderMountains (float x, float z) const;
    float lakeBasin       (float x, float z) const;
    float addIslands      (float x, float z) const;

    // Colour
    glm::vec3 sampleColor (float y, float x, float z) const;

    // Noise
    float perlinNoise (float x, float y) const;
    float fbm         (float x, float z, int octaves) const;
    static float fade (float t);
    static float lerp (float a, float b, float t);
    static float grad (int hash, float x, float y);
};