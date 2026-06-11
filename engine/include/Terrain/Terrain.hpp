#pragma once
#include "Objects/Interfaces/Drawable.hpp"
#include <glm/glm.hpp>
#include <vector>

class Terrain : public Drawable
{
public:
    Terrain(int width, int depth, float spacing);

    float     getHeight      (float x, float z) const;
    glm::vec3 getNormal      (float x, float z) const;
    glm::vec3 getIslandCenter(int index)         const;

    // Returns true if (x,z) is on land (outside sand+lake zone).
    // Use this to safely place trees, rocks, etc.
    //   de_threshold: elliptic distance threshold – 1.20 = just past beach
    bool isOnLand(float x, float z, float de_threshold = 1.20f) const {
        float ex = x / m_lakeRX, ez = z / m_lakeRZ;
        return (ex*ex + ez*ez) > (de_threshold * de_threshold);
    }

    // Semi-axes of the lake ellipse (world units) – expose for SceneBuilder
    float lakeRX() const { return m_lakeRX; }
    float lakeRZ() const { return m_lakeRZ; }

    static constexpr float WATER_LEVEL = -10.2f;

    // Island descriptor (world-space units after scaling)
    struct IslandDef {
        float angle, radius, height, size;
        bool  house;
    };

protected:
    // Drawable interface
    void buildMesh()      override;
    void computeNormals() override;
    void setupBuffers()   override;
    void draw(const DrawContext& ctx) override;

private:
    // ----- Water mesh (separate VAO – plain struct, owns its own GL handles) ----
    void buildWater();
    void setupWaterBuffers();
    void drawWater();

    std::vector<Vertex> m_waterVertices;
    std::vector<GLuint> m_waterIndices;
    struct WaterBuffers { GLuint VAO=0, VBO=0, EBO=0; } m_waterBuffers;

    // ----- Terrain dimensions -----------------------------------------------
    // NOTE: m_vertices, m_indices, m_buffers are inherited from Drawable.
    //       Do NOT redeclare them here.
    int   m_width, m_depth;
    float m_spacing;
    float m_halfW, m_halfD;   // world-space half-extents

    float m_lakeRX;           // ellipse semi-axis X (= 0.42 * m_halfW)
    float m_lakeRZ;           // ellipse semi-axis Z (= 0.28 * m_halfD)

    // ----- Island table (lazily built from fraction constants) ---------------
    mutable std::vector<IslandDef> m_islandCache;
    const std::vector<IslandDef>& islandTable() const;

    // ----- Height pipeline --------------------------------------------------
    float sampleHeight    (float x, float z) const;
    float sampleHeightFull(float x, float z) const;
    float borderMountains (float x, float z) const;
    float lakeBasin       (float x, float z) const;
    float addIslands      (float x, float z) const;
    float innerField      (float x, float z) const;

    // ----- Colour -----------------------------------------------------------
    glm::vec4 sampleColor(float y, float x, float z) const;

    // ----- Noise ------------------------------------------------------------
    float perlinNoise(float x, float y) const;
    float fbm        (float x, float z, int octaves) const;
    static float fade(float t);
    static float lerp(float a, float b, float t);
    static float grad(int hash, float x, float y);
};