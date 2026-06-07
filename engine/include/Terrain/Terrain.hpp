#pragma once

#include "Objects/Interfaces/Drawable.hpp"
#include <glm/glm.hpp>
#include <vector>

class Terrain : public Drawable
{
public:
    // width/depth = number of quads along each axis (128 recommended)
    // spacing     = world-space distance between grid points (e.g. 2.0f)
    Terrain(int width, int depth, float spacing);

    // ---------- Public queries used by scene objects ----------

    // Height at any world (x, z) position
    float     getHeight(float x, float z) const;

    // Approximate surface normal at (x, z)
    glm::vec3 getNormal(float x, float z) const;

    // Returns the world-space centre of island[index] (y = terrain height there).
    // index 0..1 are the two "house" islands; others are decorative.
    glm::vec3 getIslandCenter(int index) const;

    // Water level – anything below this is lake / sea
    static constexpr float WATER_LEVEL = -10.2f;

protected:
    void buildMesh()     override;
    void computeNormals()override;
    void setupBuffers()  override;
    void draw(const DrawContext& ctx) override;

private:
    int   m_width;
    int   m_depth;
    float m_spacing;

    std::vector<Vertex>  m_vertices;
    std::vector<GLuint>  m_indices;
    struct { GLuint VAO, VBO, EBO; } m_buffers = {0, 0, 0};

    // ---------- Height / colour helpers ----------
    float     sampleHeight    (float x, float z) const;
    float     borderMountains (float x, float z) const;
    float     lakeBasin       (float x, float z) const; // returns lake-floor height
    float     addIslands      (float x, float z) const;
    float     perlinNoise     (float x, float z) const;
    float     fbm             (float x, float z, int octaves) const; // fractal noise

    glm::vec3 sampleColor     (float y, float x, float z) const;

    // ---------- Noise helpers ----------
    static float fade(float t);
    static float lerp (float a, float b, float t);
    static float grad (int hash, float x, float y);

    // ---------- Island table (shared between height & query) ----------
    struct IslandDef {
        float angle;   // polar angle from lake centre
        float radius;  // distance from lake centre
        float height;  // peak height above lake floor
        float size;    // island footprint radius
        bool  house;   // true = place a house here
    };
    static const std::vector<IslandDef>& islandTable();
};