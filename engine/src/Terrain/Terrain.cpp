#include "Terrain/Terrain.hpp"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>
#include <cmath>
#include <vector>
#include <random>
#include "Objects/Interfaces/Drawable.hpp"

// ============================================================================
//  Permutation table for Perlin noise (initialised once)
// ============================================================================
static int p[512];

static void initPermutation()
{
    static bool done = false;
    if (done) return;
    std::mt19937 rng(42u); // fixed seed → deterministic terrain
    int perm[256];
    for (int i = 0; i < 256; ++i) perm[i] = i;
    std::shuffle(perm, perm + 256, rng);
    for (int i = 0; i < 512; ++i) p[i] = perm[i & 255];
    done = true;
}

// ============================================================================
//  Island definition table
//  The first two entries (house == true) receive the two cottages.
//  They sit on opposite sides of the lake so the lake separates them.
// ============================================================================
const std::vector<Terrain::IslandDef>& Terrain::islandTable()
{
    static const std::vector<IslandDef> table = {
        //  angle        radius  height  size   house?
        {  0.0f,         22.0f,  3.5f,  6.0f,  true  },  // 0 – east  cottage
        {  3.14159f,     22.0f,  3.5f,  6.0f,  true  },  // 1 – west  cottage
        {  1.5708f,      28.0f,  2.0f,  4.5f,  false },  // north
        {  4.71239f,     26.0f,  2.2f,  4.8f,  false },  // south
        {  0.7854f,      32.0f,  1.6f,  3.5f,  false },  // north-east
        {  2.35619f,     30.0f,  1.8f,  4.0f,  false },  // north-west
        {  3.92699f,     28.0f,  1.5f,  3.2f,  false },  // south-west
        {  5.49779f,     31.0f,  1.7f,  3.8f,  false },  // south-east
        {  0.3927f,      18.0f,  1.2f,  2.5f,  false },  // tiny pebble NE
        {  2.7489f,      19.0f,  1.0f,  2.2f,  false },  // tiny pebble NW
    };
    return table;
}

// ============================================================================
//  Constructor
// ============================================================================
Terrain::Terrain(int width, int depth, float spacing)
    : m_width(width), m_depth(depth), m_spacing(spacing)
{
    initPermutation();
    buildMesh();
    computeNormals();
    setupBuffers();
}

// ============================================================================
//  Master height sampler
// ============================================================================
float Terrain::sampleHeight(float x, float z) const
{
    // 1. Mountains on every border
    float y = borderMountains(x, z);

    // 2. Carve out the central lake basin (overrides mountains near centre)
    float basin = lakeBasin(x, z);
    if (basin < y) y = basin;   // the basin digs down

    // 3. Raise islands inside the lake
    y += addIslands(x, z);

    return y;
}

// ============================================================================
//  Border mountains
//  Strategy: use a radial "border factor" that is 1 on the perimeter and
//  falls to 0 approaching the lake zone.  Layer three octaves of Perlin
//  noise to break the regularity.
// ============================================================================
float Terrain::borderMountains(float x, float z) const
{
    float halfW = (m_width  * m_spacing) * 0.5f;
    float halfD = (m_depth  * m_spacing) * 0.5f;

    // Normalised distance to the nearest edge  (0 = centre, 1 = on border)
    float nx = 1.0f - std::abs(x) / halfW;   // 0 at wall, 1 at centre
    float nz = 1.0f - std::abs(z) / halfD;
    float distToCenter = std::min(nx, nz);    // 0 at nearest wall
    float borderFactor = glm::clamp(1.0f - distToCenter * 1.4f, 0.0f, 1.0f);
    borderFactor = std::pow(borderFactor, 0.7f); // sharpen wall

    if (borderFactor < 0.001f) return 0.0f;

    // FBM noise for natural mountain silhouette
    float n = fbm(x * 0.05f, z * 0.05f, 5);  // [0..1]

    // Directional asymmetry: north/south peaks are taller
    float angle  = std::atan2(z, x);
    float aspect = 0.75f + 0.25f * std::cos(angle * 2.0f);

    float height = borderFactor * aspect * (12.0f * n + 3.0f);
    return height;
}

// ============================================================================
//  Lake basin
//  A smooth radial depression centred at the origin.
//  Returns a height value; where it is lower than the mountain value it wins.
// ============================================================================
float Terrain::lakeBasin(float x, float z) const
{
    const float lakeRadius = 80.0f;   // full lake footprint radius
    const float shoreWidth = 30.0f;   // transition band shore→lake
    const float lakeFloor  = WATER_LEVEL - 1.5f;

    float dist = std::sqrt(x * x + z * z);

    if (dist >= lakeRadius + shoreWidth) {
        // Outside the lake influence – return a very high value so it never wins
        return 1000.0f;
    }

    if (dist <= lakeRadius) {
        // Fully inside lake – flat floor with tiny noise for texture
        float ripple = perlinNoise(x * 0.3f, z * 0.3f) * 0.15f;
        return lakeFloor + ripple;
    }

    // Shore transition: smooth blend from lakeFloor up to ground level (0)
    float t = (dist - lakeRadius) / shoreWidth;  // 0 (lake edge) → 1 (land)
    t = fade(t);  // smoothstep
    float shoreHeight = WATER_LEVEL + 0.6f + t * 6.0f;
    return shoreHeight;
}

// ============================================================================
//  Islands
//  Each island is a smooth cosine dome above the lake floor.
//  Returns the additive height contribution (0 outside all islands).
// ============================================================================
float Terrain::addIslands(float x, float z) const
{
    const float lakeFloor = WATER_LEVEL - 1.5f;
    float extra = 0.0f;

    for (const auto& isl : islandTable()) {
        float cx = std::cos(isl.angle) * isl.radius;
        float cz = std::sin(isl.angle) * isl.radius;
        float dist = std::sqrt((x - cx) * (x - cx) + (z - cz) * (z - cz));

        if (dist >= isl.size) continue;

        float t = 1.0f - dist / isl.size;           // 1 at peak, 0 at edge
        float dome = isl.height * (t * t * (3.0f - 2.0f * t)); // smoothstep dome

        // Add micro-roughness so the island doesn't look plastic
        dome += perlinNoise(x * 1.8f, z * 1.8f) * 0.25f * t;

        // Convert dome height: it is relative to the lake floor
        float worldH = lakeFloor + dome;
        // We return an additive value; sampleHeight adds it on top of the basin
        // so we just return dome (addIslands is called after the basin clamps y)
        extra = std::max(extra, dome);
    }

    return extra;
}

// ============================================================================
//  Public query: centre of island[index] in world space
// ============================================================================
glm::vec3 Terrain::getIslandCenter(int index) const
{
    const auto& table = islandTable();
    if (index < 0 || index >= (int)table.size()) return glm::vec3(0.0f);

    const auto& isl = table[index];
    float cx = std::cos(isl.angle) * isl.radius;
    float cz = std::sin(isl.angle) * isl.radius;
    float cy = getHeight(cx, cz);
    return glm::vec3(cx, cy, cz);
}

// ============================================================================
//  Perlin helpers
// ============================================================================
float Terrain::fade(float t) {
    return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

float Terrain::lerp(float a, float b, float t) {
    return a + t * (b - a);
}

float Terrain::grad(int hash, float x, float y) {
    int h = hash & 7;
    float u = h < 4 ? x : y;
    float v = h < 4 ? y : x;
    return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
}

float Terrain::perlinNoise(float x, float y) const
{
    int xi = (int)std::floor(x) & 255;
    int yi = (int)std::floor(y) & 255;
    float xf = x - std::floor(x);
    float yf = y - std::floor(y);

    float u = fade(xf);
    float v = fade(yf);

    int aa = p[p[xi]     + yi    ];
    int ab = p[p[xi]     + yi + 1];
    int ba = p[p[xi + 1] + yi    ];
    int bb = p[p[xi + 1] + yi + 1];

    float x1 = lerp(grad(aa, xf,       yf    ), grad(ba, xf - 1.0f, yf    ), u);
    float x2 = lerp(grad(ab, xf,       yf - 1), grad(bb, xf - 1.0f, yf - 1), u);
    return (lerp(x1, x2, v) + 1.0f) * 0.5f; // [0, 1]
}

// Fractal Brownian Motion – stacks octaves of Perlin for richer detail
float Terrain::fbm(float x, float z, int octaves) const
{
    float value  = 0.0f;
    float amp    = 0.5f;
    float freq   = 1.0f;
    float maxVal = 0.0f;
    for (int i = 0; i < octaves; ++i) {
        value  += perlinNoise(x * freq, z * freq) * amp;
        maxVal += amp;
        amp  *= 0.5f;
        freq *= 2.0f;
    }
    return value / maxVal; // [0, 1]
}

// ============================================================================
//  Color mapping
//  Uses height + a touch of fbm for natural variation on slopes
// ============================================================================
glm::vec3 Terrain::sampleColor(float y, float x, float z) const
{
    // Lake / water colours
    if (y < WATER_LEVEL - 0.5f)
        return glm::vec3(0.08f, 0.15f, 0.45f);  // deep lake
    if (y < WATER_LEVEL)
        return glm::vec3(0.15f, 0.45f, 0.75f);  // shallow / shore water

    // Sandy beach immediately above water line
    if (y < WATER_LEVEL + 0.8f)
        return glm::vec3(0.75f, 0.65f, 0.45f);

    // Grass → rock → snow based on normalised height
    // Mountains peak around y = 15; normalise to [0,1] in that range
    float t = glm::clamp((y - (WATER_LEVEL + 0.8f)) / 14.0f, 0.0f, 1.0f);

    // Small noise patch for colour variation
    float n = fbm(x * 0.08f, z * 0.08f, 3);
    t = glm::clamp(t + (n - 0.5f) * 0.15f, 0.0f, 1.0f);

    const glm::vec3 grass(0.28f, 0.58f, 0.18f);
    const glm::vec3 brown(0.55f, 0.38f, 0.20f);
    const glm::vec3 grey (0.50f, 0.48f, 0.46f);
    const glm::vec3 snow (0.92f, 0.92f, 0.95f);

    if (t < 0.30f) return glm::mix(grass, brown, t / 0.30f);
    if (t < 0.65f) return glm::mix(brown, grey,  (t - 0.30f) / 0.35f);
    return          glm::mix(grey,  snow,  (t - 0.65f) / 0.35f);
}

// ============================================================================
//  Mesh construction
// ============================================================================
void Terrain::buildMesh()
{
    m_vertices.clear();
    m_indices.clear();

    // World-space extents (terrain is centred at origin)
    float halfW = m_width  * m_spacing * 0.5f;
    float halfD = m_depth  * m_spacing * 0.5f;

    for (int z = 0; z <= m_depth; ++z) {
        for (int x = 0; x <= m_width; ++x) {
            float wx = x * m_spacing - halfW;
            float wz = z * m_spacing - halfD;
            float wy = sampleHeight(wx, wz);

            Vertex v;
            v.position = { wx, wy, wz };
            v.normal   = { 0.0f, 1.0f, 0.0f }; // recomputed later
            v.texcoord = { float(x) / m_width, float(z) / m_depth };
            v.color    = sampleColor(wy, wx, wz);
            m_vertices.push_back(v);
        }
    }

    // Two triangles per quad
    for (int z = 0; z < m_depth; ++z) {
        for (int x = 0; x < m_width; ++x) {
            GLuint tl = (GLuint)( z      * (m_width + 1) + x);
            GLuint tr = tl + 1;
            GLuint bl = (GLuint)((z + 1) * (m_width + 1) + x);
            GLuint br = bl + 1;

            m_indices.push_back(tl); m_indices.push_back(bl); m_indices.push_back(tr);
            m_indices.push_back(tr); m_indices.push_back(bl); m_indices.push_back(br);
        }
    }
}

// ============================================================================
//  Normal computation (area-weighted face normals → vertex normals)
// ============================================================================
void Terrain::computeNormals()
{
    for (auto& v : m_vertices) v.normal = glm::vec3(0.0f);

    for (size_t i = 0; i < m_indices.size(); i += 3) {
        GLuint i0 = m_indices[i], i1 = m_indices[i+1], i2 = m_indices[i+2];

        const glm::vec3& p0 = m_vertices[i0].position;
        const glm::vec3& p1 = m_vertices[i1].position;
        const glm::vec3& p2 = m_vertices[i2].position;

        glm::vec3 face = glm::cross(p1 - p0, p2 - p0); // magnitude = 2×area

        m_vertices[i0].normal += face;
        m_vertices[i1].normal += face;
        m_vertices[i2].normal += face;
    }

    for (auto& v : m_vertices) {
        float len = glm::length(v.normal);
        v.normal = (len > 1e-5f) ? v.normal / len : glm::vec3(0, 1, 0);
    }
}

// ============================================================================
//  OpenGL buffer setup
// ============================================================================
void Terrain::setupBuffers()
{
    glGenVertexArrays(1, &m_buffers.VAO);
    glGenBuffers(1, &m_buffers.VBO);
    glGenBuffers(1, &m_buffers.EBO);

    glBindVertexArray(m_buffers.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_buffers.VBO);
    glBufferData(GL_ARRAY_BUFFER,
                 (GLsizeiptr)(m_vertices.size() * sizeof(Vertex)),
                 m_vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 (GLsizeiptr)(m_indices.size() * sizeof(GLuint)),
                 m_indices.data(), GL_STATIC_DRAW);

    // location 0 – position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);

    // location 1 – normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    // location 2 – texcoord
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, texcoord));
    glEnableVertexAttribArray(2);

    // location 3 – colour
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(3);

    glBindVertexArray(0);
}

// ============================================================================
//  Draw
// ============================================================================
void Terrain::draw(const DrawContext& /*ctx*/)
{
    glBindVertexArray(m_buffers.VAO);
    glDrawElements(GL_TRIANGLES, (GLsizei)m_indices.size(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

// ============================================================================
//  Public queries
// ============================================================================
float Terrain::getHeight(float x, float z) const
{
    return sampleHeight(x, z);
}

glm::vec3 Terrain::getNormal(float x, float z) const
{
    constexpr float eps = 0.2f;
    float r = sampleHeight(x + eps, z);
    float l = sampleHeight(x - eps, z);
    float f = sampleHeight(x, z + eps);
    float b = sampleHeight(x, z - eps);
    return glm::normalize(glm::vec3(l - r, 2.0f * eps, b - f));
}