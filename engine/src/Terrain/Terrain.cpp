#include "Terrain/Terrain.hpp"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>
#include <cmath>
#include <vector>
#include <random>
#include "Objects/Interfaces/Drawable.hpp"

// ============================================================================
//  All magic numbers that depend on terrain size are derived at runtime from
//  m_halfW (= m_width * m_spacing * 0.5f).  Nothing is hardcoded in world
//  units – so the same file works for any (width, depth, spacing) triple.
// ============================================================================

// ============================================================================
//  Permutation table  (deterministic, seed 42)
// ============================================================================
static int p[512];
static void initPermutation()
{
    static bool done = false;
    if (done) return;
    std::mt19937 rng(42u);
    int perm[256];
    for (int i = 0; i < 256; ++i) perm[i] = i;
    std::shuffle(perm, perm + 256, rng);
    for (int i = 0; i < 512; ++i) p[i] = perm[i & 255];
    done = true;
}

// ============================================================================
//  Island table  – radii and sizes stored as FRACTIONS of m_halfW.
//  Converted to world units inside islandDefs().
// ============================================================================
//  fAngle   fRadius  fHeight  fSize   house
//  (rad)    (×half)  (world)  (×half)
static constexpr struct RawIsland {
    float angle, fRadius, height, fSize;
    bool  house;
} RAW_ISLANDS[] = {
    { 0.0f,       0.40f,  6.0f,  0.026f, true  },  // 0 east  – cottage
    { 3.14159f,   0.40f,  6.0f,  0.026f, true  },  // 1 west  – cottage
    { 1.5708f,    0.50f,  4.0f,  0.018f, false },   // north
    { 4.71239f,   0.48f,  4.5f,  0.020f, false },   // south
    { 0.7854f,    0.55f,  3.0f,  0.014f, false },   // NE
    { 2.35619f,   0.52f,  3.5f,  0.016f, false },   // NW
    { 3.92699f,   0.50f,  3.2f,  0.013f, false },   // SW
    { 5.49779f,   0.53f,  3.8f,  0.015f, false },   // SE
    { 0.3927f,    0.33f,  2.0f,  0.010f, false },   // tiny NE
    { 2.7489f,    0.35f,  1.8f,  0.009f, false },   // tiny NW
};
static constexpr int NUM_ISLANDS = (int)(sizeof(RAW_ISLANDS) / sizeof(RAW_ISLANDS[0]));

// Convert raw fractional island to world-space IslandDef
const std::vector<Terrain::IslandDef>& Terrain::islandTable() const
{
    // Lazy-initialise; keyed on m_halfW so it stays valid after construction.
    if (!m_islandCache.empty()) return m_islandCache;

    m_islandCache.reserve(NUM_ISLANDS);
    for (int i = 0; i < NUM_ISLANDS; ++i) {
        const auto& r = RAW_ISLANDS[i];
        IslandDef d;
        d.angle  = r.angle;
        d.radius = r.fRadius * m_halfW;
        d.height = r.height;
        d.size   = r.fSize   * m_halfW;
        d.house  = r.house;
        m_islandCache.push_back(d);
    }
    return m_islandCache;
}

// ============================================================================
//  Constructor
// ============================================================================
Terrain::Terrain(int width, int depth, float spacing)
    : m_width(width), m_depth(depth), m_spacing(spacing),
      m_halfW(width  * spacing * 0.5f),
      m_halfD(depth  * spacing * 0.5f)
{
    initPermutation();
    buildMesh();
    computeNormals();
    setupBuffers();
}

// ============================================================================
//  Master height sampler
//  Pipeline: mountains → lake basin carves in → islands push up
// ============================================================================
float Terrain::sampleHeight(float x, float z) const
{
    float y     = borderMountains(x, z);
    float basin = lakeBasin(x, z);
    if (basin < y) y = basin;      // basin digs below mountains
    y += addIslands(x, z);         // islands sit on top of basin floor
    return y;
}

// ============================================================================
//  Border mountains
//
//  distToCenter ∈ [0, 1]:
//    0 → vertex is at the nearest wall
//    1 → vertex is at the terrain centre
//
//  borderFactor is 1 at the wall and drops to 0 well before the lake edge.
//  With mult=1.65 and exp=1.5, bf reaches 0 when distToCenter ≈ 0.61,
//  which corresponds to ~195 u from centre on a 320-unit half-terrain –
//  safely outside the lake radius (121 u).
// ============================================================================
float Terrain::borderMountains(float x, float z) const
{
    float nx = 1.0f - std::abs(x) / m_halfW;  // 0 at east/west walls
    float nz = 1.0f - std::abs(z) / m_halfD;  // 0 at north/south walls
    float distToCenter = std::min(nx, nz);     // 0 at nearest wall, 1 at centre

    float borderFactor = glm::clamp(1.0f - distToCenter * 1.65f, 0.0f, 1.0f);
    borderFactor = std::pow(borderFactor, 1.5f); // sharpens the mountain band

    if (borderFactor < 0.001f) return 0.0f;

    // FBM frequency: ~3 noise periods across the half-terrain
    float fx = x * (3.0f / m_halfW);
    float fz = z * (3.0f / m_halfD);
    float n = fbm(fx, fz, 5);    // rich fractal detail, range [0, 1]

    // Asymmetry: north/south walls slightly taller
    float angle  = std::atan2(z, x);
    float aspect = 0.80f + 0.20f * std::cos(angle * 2.0f);

    // Max mountain peak ≈ 25 world units above 0
    return borderFactor * aspect * (20.0f * n + 5.0f);
}

// ============================================================================
//  Lake basin
//
//  Proportions (all relative to m_halfW):
//    lakeRadius = 0.38 × halfW   → lake occupies 38% of half-terrain
//    shoreWidth = 0.10 × halfW   → smooth shore band
//  These numbers are chosen so the lake is clearly visible from any direction
//  and the border mountains do NOT reach into the lake area.
// ============================================================================
float Terrain::lakeBasin(float x, float z) const
{
    const float lakeRadius = 0.38f * m_halfW;
    const float shoreWidth = 0.10f * m_halfW;
    const float lakeFloor  = WATER_LEVEL - 2.0f;   // flat lake bed

    float dist = std::sqrt(x * x + z * z);

    if (dist >= lakeRadius + shoreWidth)
        return 1e6f;   // outside lake zone – very high, never wins

    if (dist <= lakeRadius) {
        // Flat lake bed with micro ripple for visual texture
        float ripple = perlinNoise(x * (1.5f / m_halfW) * 60.f,
                                   z * (1.5f / m_halfD) * 60.f) * 0.2f;
        return lakeFloor + ripple;
    }

    // Shore ramp: smoothly rises from lakeFloor to ground level
    float t = (dist - lakeRadius) / shoreWidth;  // [0, 1]
    t = fade(t);
    return lakeFloor + t * (8.0f - lakeFloor);   // rises to ~8 u (grass level)
}

// ============================================================================
//  Islands
//  Returns the extra height added on top of the lake basin floor.
//  The dome height is in world units (metres), independent of spacing.
// ============================================================================
float Terrain::addIslands(float x, float z) const
{
    float extra = 0.0f;
    for (const auto& isl : islandTable()) {
        float cx = std::cos(isl.angle) * isl.radius;
        float cz = std::sin(isl.angle) * isl.radius;
        float dist = std::sqrt((x - cx) * (x - cx) + (z - cz) * (z - cz));

        if (dist >= isl.size) continue;

        float t = 1.0f - dist / isl.size;
        float dome = isl.height * t * t * (3.0f - 2.0f * t);  // smoothstep

        // Fine roughness on the island surface
        float fx = x * (8.0f / m_halfW) * 40.f;
        float fz = z * (8.0f / m_halfD) * 40.f;
        dome += perlinNoise(fx, fz) * 0.4f * t;

        extra = std::max(extra, dome);
    }
    return extra;
}

// ============================================================================
//  Public: centre of island[index] in world space
// ============================================================================
glm::vec3 Terrain::getIslandCenter(int index) const
{
    const auto& table = islandTable();
    if (index < 0 || index >= (int)table.size()) return glm::vec3(0.0f);
    const auto& isl = table[index];
    float cx = std::cos(isl.angle) * isl.radius;
    float cz = std::sin(isl.angle) * isl.radius;
    return glm::vec3(cx, getHeight(cx, cz), cz);
}

// ============================================================================
//  Colour mapping
//
//  The colour ramp is calibrated against the actual height range:
//    < WATER_LEVEL      → lake (blue)
//    WATER_LEVEL..+1.5  → beach (sand)
//    above              → grass → brown → rock → snow  (peak ~25u)
// ============================================================================
glm::vec3 Terrain::sampleColor(float y, float x, float z) const
{
    if (y < WATER_LEVEL - 1.0f)
        return glm::vec3(0.07f, 0.13f, 0.42f);  // deep lake
    if (y < WATER_LEVEL)
        return glm::vec3(0.14f, 0.42f, 0.72f);  // shallow lake
    if (y < WATER_LEVEL + 1.5f)
        return glm::vec3(0.76f, 0.66f, 0.46f);  // sandy beach

    // Normalise into [0,1] over the mountain height range
    // Mountains peak at ~25u; WATER_LEVEL + 1.5 is the beach top.
    const float landBase = WATER_LEVEL + 1.5f;
    const float mtnPeak  = 25.0f;
    float t = glm::clamp((y - landBase) / (mtnPeak - landBase), 0.0f, 1.0f);

    // Add a small noise patch so solid bands don't appear
    float n = fbm(x * (2.0f / m_halfW) * 40.f,
                  z * (2.0f / m_halfD) * 40.f, 3);
    t = glm::clamp(t + (n - 0.5f) * 0.12f, 0.0f, 1.0f);

    const glm::vec3 grass(0.27f, 0.56f, 0.17f);
    const glm::vec3 brown(0.54f, 0.37f, 0.19f);
    const glm::vec3 grey (0.50f, 0.48f, 0.46f);
    const glm::vec3 snow (0.93f, 0.93f, 0.96f);

    if (t < 0.28f) return glm::mix(grass, brown, t / 0.28f);
    if (t < 0.62f) return glm::mix(brown, grey,  (t - 0.28f) / 0.34f);
    return          glm::mix(grey,  snow,  (t - 0.62f) / 0.38f);
}

// ============================================================================
//  Noise helpers
// ============================================================================
float Terrain::fade(float t)           { return t*t*t*(t*(t*6.f-15.f)+10.f); }
float Terrain::lerp(float a,float b,float t) { return a + t*(b-a); }
float Terrain::grad(int hash,float x,float y) {
    int h = hash & 7;
    float u = h < 4 ? x : y;
    float v = h < 4 ? y : x;
    return ((h&1)?-u:u)+((h&2)?-v:v);
}

float Terrain::perlinNoise(float x, float y) const
{
    int xi = (int)std::floor(x) & 255;
    int yi = (int)std::floor(y) & 255;
    float xf = x - std::floor(x);
    float yf = y - std::floor(y);
    float u = fade(xf), v = fade(yf);
    int aa=p[p[xi]+yi], ab=p[p[xi]+yi+1], ba=p[p[xi+1]+yi], bb=p[p[xi+1]+yi+1];
    float x1 = lerp(grad(aa,xf,yf),    grad(ba,xf-1,yf),   u);
    float x2 = lerp(grad(ab,xf,yf-1),  grad(bb,xf-1,yf-1), u);
    return (lerp(x1,x2,v)+1.f)*0.5f;  // [0,1]
}

float Terrain::fbm(float x, float z, int octaves) const
{
    float val=0,amp=0.5f,freq=1.f,maxV=0;
    for(int i=0;i<octaves;++i){
        val  += perlinNoise(x*freq,z*freq)*amp;
        maxV += amp; amp*=0.5f; freq*=2.f;
    }
    return val/maxV;  // [0,1]
}

// ============================================================================
//  Mesh build
// ============================================================================
void Terrain::buildMesh()
{
    m_vertices.clear();
    m_indices.clear();

    for (int z = 0; z <= m_depth; ++z) {
        for (int x = 0; x <= m_width; ++x) {
            float wx = x * m_spacing - m_halfW;
            float wz = z * m_spacing - m_halfD;
            float wy = sampleHeight(wx, wz);

            Vertex v;
            v.position = { wx, wy, wz };
            v.normal   = { 0.f, 1.f, 0.f };
            v.texcoord = { float(x)/m_width, float(z)/m_depth };
            v.color    = sampleColor(wy, wx, wz);
            m_vertices.push_back(v);
        }
    }

    for (int z = 0; z < m_depth; ++z)
        for (int x = 0; x < m_width; ++x) {
            GLuint tl=(GLuint)(z*(m_width+1)+x), tr=tl+1;
            GLuint bl=(GLuint)((z+1)*(m_width+1)+x), br=bl+1;
            m_indices.push_back(tl); m_indices.push_back(bl); m_indices.push_back(tr);
            m_indices.push_back(tr); m_indices.push_back(bl); m_indices.push_back(br);
        }
}

// ============================================================================
//  Normal computation
// ============================================================================
void Terrain::computeNormals()
{
    for (auto& v : m_vertices) v.normal = glm::vec3(0.f);
    for (size_t i=0; i<m_indices.size(); i+=3) {
        GLuint i0=m_indices[i],i1=m_indices[i+1],i2=m_indices[i+2];
        glm::vec3 e1=m_vertices[i1].position-m_vertices[i0].position;
        glm::vec3 e2=m_vertices[i2].position-m_vertices[i0].position;
        glm::vec3 fn=glm::cross(e1,e2);
        m_vertices[i0].normal+=fn; m_vertices[i1].normal+=fn; m_vertices[i2].normal+=fn;
    }
    for (auto& v : m_vertices) {
        float l=glm::length(v.normal);
        v.normal=(l>1e-5f)?v.normal/l:glm::vec3(0,1,0);
    }
}

// ============================================================================
//  OpenGL buffers
// ============================================================================
void Terrain::setupBuffers()
{
    glGenVertexArrays(1,&m_buffers.VAO);
    glGenBuffers(1,&m_buffers.VBO);
    glGenBuffers(1,&m_buffers.EBO);
    glBindVertexArray(m_buffers.VAO);

    glBindBuffer(GL_ARRAY_BUFFER,m_buffers.VBO);
    glBufferData(GL_ARRAY_BUFFER,(GLsizeiptr)(m_vertices.size()*sizeof(Vertex)),
                 m_vertices.data(),GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,m_buffers.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,(GLsizeiptr)(m_indices.size()*sizeof(GLuint)),
                 m_indices.data(),GL_STATIC_DRAW);

    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex,position));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex,normal));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex,texcoord));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex,color));
    glEnableVertexAttribArray(3);

    glBindVertexArray(0);
}

// ============================================================================
//  Draw / queries
// ============================================================================
void Terrain::draw(const DrawContext& /*ctx*/)
{
    glBindVertexArray(m_buffers.VAO);
    glDrawElements(GL_TRIANGLES,(GLsizei)m_indices.size(),GL_UNSIGNED_INT,nullptr);
    glBindVertexArray(0);
}

float Terrain::getHeight(float x, float z) const { return sampleHeight(x,z); }

glm::vec3 Terrain::getNormal(float x, float z) const
{
    float eps=m_spacing*0.5f;
    float r=sampleHeight(x+eps,z), l=sampleHeight(x-eps,z);
    float f=sampleHeight(x,z+eps), b=sampleHeight(x,z-eps);
    return glm::normalize(glm::vec3(l-r, 2.f*eps, b-f));
}