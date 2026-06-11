#include "Terrain/Terrain.hpp"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>
#include <cmath>
#include <vector>
#include <random>
#include "Objects/Interfaces/Drawable.hpp"

// ============================================================================
//  Permutation table (deterministic, seed 42)
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
//  Island table – radii and sizes as FRACTIONS of m_halfW
// ============================================================================
static constexpr struct RawIsland {
    float angle, fRadius, height, fSize;
    bool  house;
} RAW_ISLANDS[] = {
    { 0.0f,       0.40f,  6.0f,  0.026f, true  },
    { 3.14159f,   0.40f,  6.0f,  0.026f, true  },
    { 1.5708f,    0.50f,  4.0f,  0.018f, false },
    { 4.71239f,   0.48f,  4.5f,  0.020f, false },
    { 0.7854f,    0.55f,  3.0f,  0.014f, false },
    { 2.35619f,   0.52f,  3.5f,  0.016f, false },
    { 3.92699f,   0.50f,  3.2f,  0.013f, false },
    { 5.49779f,   0.53f,  3.8f,  0.015f, false },
    { 0.3927f,    0.33f,  2.0f,  0.010f, false },
    { 2.7489f,    0.35f,  1.8f,  0.009f, false },
};
static constexpr int NUM_ISLANDS = (int)(sizeof(RAW_ISLANDS) / sizeof(RAW_ISLANDS[0]));

const std::vector<Terrain::IslandDef>& Terrain::islandTable() const
{
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
      m_halfD(depth  * spacing * 0.5f),
      m_lakeRX(0.42f * width  * spacing * 0.5f),
      m_lakeRZ(0.28f * depth  * spacing * 0.5f)
{
    initPermutation();
    buildMesh();
    computeNormals();
    setupBuffers();
    buildWater();
    setupWaterBuffers();
}

// ============================================================================
//  Master height sampler
//  Pipeline: mountains → lake basin carves in → islands push up
// ============================================================================
float Terrain::sampleHeight(float x, float z) const
{
    float y     = borderMountains(x, z);
    float basin = lakeBasin(x, z);
    if (basin < y) y = basin;
    y += addIslands(x, z);
    return y;
}

// ============================================================================
//  Border mountains  –  domain-warped FBM for organic shapes
//
//  Domain warping: before sampling FBM for height, offset (fx,fz) by another
//  FBM layer.  This makes the ridge lines twist and fold like real mountains
//  instead of being smooth elliptical gradients.
//
//  edgeDist (Chebyshev): 0 at centre, 1 at any wall.
//  Mountains rise for edgeDist > 0.42, leaving the inner 42% flat/lake.
// ============================================================================
float Terrain::borderMountains(float x, float z) const
{
    float nx = std::abs(x) / m_halfW;
    float nz = std::abs(z) / m_halfD;
    float edgeDist = std::max(nx, nz);

    // Mountains start at edgeDist > 0.30 (wide band) and peak at the wall.
    float borderFactor = glm::clamp((edgeDist - 0.30f) / 0.70f, 0.0f, 1.0f);
    // Gentle exponent — the mountain zone is wide and consistently tall
    borderFactor = std::pow(borderFactor, 1.2f);

    if (borderFactor < 0.001f) return 0.0f;

    // --- Domain warping (two layers) ---
    float fx0 = x * (3.2f / m_halfW);
    float fz0 = z * (3.2f / m_halfD);
    float warpAmp = 0.70f;
    float wx2 = fbm(fx0 + 0.3f, fz0 + 1.7f, 4) * warpAmp;
    float wz2 = fbm(fx0 + 5.2f, fz0 + 3.4f, 4) * warpAmp;
    float fx = fx0 + wx2;
    float fz = fz0 + wz2;
    float n = fbm(fx, fz, 7);   // 7 octaves for rich detail

    // Ridge sharpening
    float ridge = 1.0f - std::abs(2.0f * n - 1.0f);
    ridge = ridge * ridge * ridge;
    n = glm::mix(n, ridge, 0.55f);

    float angle  = std::atan2(z, x);
    float aspect = 0.85f + 0.15f * std::cos(angle * 2.0f);

    // Peak ≈ 80u; minimum at wall ≈ 18u — very visible even from far away
    return borderFactor * aspect * (58.0f * n + 18.0f);
}

// ============================================================================
//  Lake basin  –  elliptical, with a soft noise-perturbed shore
//
//  The shore width varies slightly around the ellipse using low-freq noise,
//  so the beach doesn't look like a perfect mathematical outline.
// ============================================================================
float Terrain::lakeBasin(float x, float z) const
{
    const float shoreW    = 0.09f * m_halfW;
    const float lakeFloor = WATER_LEVEL - 2.5f;  // deeper for contrast

    float ex = x / m_lakeRX;
    float ez = z / m_lakeRZ;
    float de = std::sqrt(ex * ex + ez * ez);

    // Perturb the shore radius slightly for an organic shoreline
    float shoreNoise = fbm(x * (1.2f / m_halfW) * 5.f,
                           z * (1.2f / m_halfD) * 5.f, 3);
    float shoreRel = (shoreW / m_lakeRX) * (0.75f + 0.50f * shoreNoise);

    if (de >= 1.0f + shoreRel)
        return 1e6f;

    if (de <= 1.0f) {
        // Lake bed: mostly flat, tiny geological ripple
        float ripple = fbm(x * (4.f / m_halfW) * 12.f,
                           z * (4.f / m_halfD) * 12.f, 3) * 0.35f;
        return lakeFloor + ripple;
    }

    // Shore ramp with smoothstep
    float t = (de - 1.0f) / shoreRel;
    t = fade(t);
    // Rises to ~7u (just above WATER_LEVEL + beach height)
    return lakeFloor + t * (7.0f - lakeFloor);
}

// ============================================================================
//  Inner field  –  gentle rolling hills between lake shore and mountains
//
//  Returns a small positive offset applied to non-lake, non-mountain terrain.
//  This avoids a perfectly flat boring meadow.
// ============================================================================
float Terrain::innerField(float x, float z) const
{
    float nx = std::abs(x) / m_halfW;
    float nz = std::abs(z) / m_halfD;
    float edgeDist = std::max(nx, nz);

    // Only active in the flat middle zone [0, 0.45]
    float fieldFactor = glm::clamp(1.0f - edgeDist / 0.45f, 0.0f, 1.0f);
    fieldFactor = fieldFactor * fieldFactor;

    // Elliptic distance – suppress hills near the lake shore
    float ex = x / m_lakeRX;
    float ez = z / m_lakeRZ;
    float de = std::sqrt(ex * ex + ez * ez);
    float lakeFade = glm::clamp((de - 1.25f) / 0.40f, 0.0f, 1.0f);

    float combined = fieldFactor * lakeFade;
    if (combined < 0.001f) return 0.0f;

    // Low-frequency rolling hills (2–3 world units amplitude)
    float hx = x * (1.8f / m_halfW);
    float hz = z * (1.8f / m_halfD);
    float hill = fbm(hx, hz, 4);   // [0,1]

    return combined * (hill * 3.5f + 0.5f);  // up to ~4u bump
}

// ============================================================================
//  Islands
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
        float dome = isl.height * t * t * (3.0f - 2.0f * t);
        float fx = x * (8.0f / m_halfW) * 40.f;
        float fz = z * (8.0f / m_halfD) * 40.f;
        dome += perlinNoise(fx, fz) * 0.4f * t;
        extra = std::max(extra, dome);
    }
    return extra;
}

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
//  Slope-aware: steep faces use a darker brown regardless of height, which
//  makes mountain sides look rocky and the flat field stay green.
//  Slope is approximated by comparing y against 4 neighbours.
// ============================================================================
glm::vec4 Terrain::sampleColor(float y, float x, float z) const
{
    // --- Below water: lake colours (invisible under water mesh, but correct) ---
    if (y < WATER_LEVEL - 1.5f)
        return glm::vec4(0.06f, 0.11f, 0.38f, 1.0f);  // deep lake bed
    if (y < WATER_LEVEL)
        return glm::vec4(0.12f, 0.35f, 0.62f, 1.0f);  // shallow lake bed

    // --- Shore: sand band keyed on elliptic distance ---
    float ex = x / m_lakeRX;
    float ez = z / m_lakeRZ;
    float de = std::sqrt(ex * ex + ez * ez);
    if (de < 1.20f)
        return glm::vec4(0.78f, 0.68f, 0.48f, 1.0f);  // sandy beach

    // --- Slope estimation (finite differences, cheap) ---
    float eps = m_spacing;
    float dydx = (sampleHeight(x + eps, z) - sampleHeight(x - eps, z)) / (2.f * eps);
    float dydz = (sampleHeight(x, z + eps) - sampleHeight(x, z - eps)) / (2.f * eps);
    float slope = std::sqrt(dydx * dydx + dydz * dydz);  // tangent of slope angle
    // Normalize: slope > ~1.5 (≈56°) is "very steep"
    float steepness = glm::clamp(slope / 1.5f, 0.0f, 1.0f);

    // --- Height ramp over land ---
    const float fieldTop = 9.0f;    // flat field ceiling
    const float mtnPeak  = 55.0f;
    float t = glm::clamp((y - fieldTop) / (mtnPeak - fieldTop), 0.0f, 1.0f);

    // Blend steepness into t: steep slopes look browner/rockier at same height
    t = glm::clamp(t + steepness * 0.35f, 0.0f, 1.0f);

    // Small noise patch to break colour banding
    float n = fbm(x * (2.0f / m_halfW) * 38.f,
                  z * (2.0f / m_halfD) * 38.f, 3);
    t = glm::clamp(t + (n - 0.5f) * 0.07f, 0.0f, 1.0f);

    // Colour stops: grass → brown → grey rock → snow
    const glm::vec3 grass(0.21f, 0.53f, 0.13f);   // rich meadow green
    const glm::vec3 brown(0.50f, 0.34f, 0.16f);   // earthy mountain base
    const glm::vec3 grey (0.49f, 0.47f, 0.45f);   // bare rock
    const glm::vec3 snow (0.92f, 0.93f, 0.96f);

    glm::vec3 rgb;
    if      (t < 0.22f) rgb = glm::mix(grass, brown, t / 0.22f);
    else if (t < 0.58f) rgb = glm::mix(brown, grey,  (t - 0.22f) / 0.36f);
    else                rgb = glm::mix(grey,  snow,  (t - 0.58f) / 0.42f);

    return glm::vec4(rgb, 1.0f);
}

// ============================================================================
//  Noise helpers
// ============================================================================
float Terrain::fade(float t) { return t*t*t*(t*(t*6.f-15.f)+10.f); }
float Terrain::lerp(float a, float b, float t) { return a + t*(b-a); }
float Terrain::grad(int hash, float x, float y) {
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
    int aa = p[p[xi]+yi],   ab = p[p[xi]+yi+1];
    int ba = p[p[xi+1]+yi], bb = p[p[xi+1]+yi+1];
    float x1 = lerp(grad(aa, xf,   yf),   grad(ba, xf-1, yf),   u);
    float x2 = lerp(grad(ab, xf,   yf-1), grad(bb, xf-1, yf-1), u);
    return (lerp(x1, x2, v) + 1.f) * 0.5f;
}

float Terrain::fbm(float x, float z, int octaves) const
{
    float val = 0.f, amp = 0.5f, freq = 1.f, maxV = 0.f;
    for (int i = 0; i < octaves; ++i) {
        val  += perlinNoise(x * freq, z * freq) * amp;
        maxV += amp;
        amp  *= 0.5f;
        freq *= 2.f;
    }
    return val / maxV;
}

// ============================================================================
//  Build terrain mesh
//  The inner field offset is baked into sampleHeight via the full pipeline.
//  We call innerField here and add it before the lake basin clamp so hills
//  don't bleed into the lake.
// ============================================================================
float Terrain::sampleHeightFull(float x, float z) const
{
    float y     = borderMountains(x, z);
    float basin = lakeBasin(x, z);
    // Inner field only applied outside the lake zone
    if (basin >= 1e5f) y += innerField(x, z);
    if (basin < y) y = basin;
    y += addIslands(x, z);
    return y;
}

void Terrain::buildMesh()
{
    m_vertices.clear();
    m_indices.clear();

    for (int z = 0; z <= m_depth; ++z) {
        for (int x = 0; x <= m_width; ++x) {
            float wx = (float)x * m_spacing - m_halfW;
            float wz = (float)z * m_spacing - m_halfD;
            float wy = sampleHeightFull(wx, wz);

            Vertex v;
            v.position = { wx, wy, wz };
            v.normal   = { 0.f, 1.f, 0.f };
            v.texcoord = { (float)x / m_width, (float)z / m_depth };
            v.color    = sampleColor(wy, wx, wz);
            m_vertices.push_back(v);
        }
    }

    for (int z = 0; z < m_depth; ++z)
        for (int x = 0; x < m_width; ++x) {
            GLuint tl = (GLuint)(z * (m_width+1) + x), tr = tl + 1;
            GLuint bl = (GLuint)((z+1) * (m_width+1) + x), br = bl + 1;
            m_indices.push_back(tl); m_indices.push_back(bl); m_indices.push_back(tr);
            m_indices.push_back(tr); m_indices.push_back(bl); m_indices.push_back(br);
        }
}

// Also update getHeight to use full pipeline
float Terrain::getHeight(float x, float z) const { return sampleHeightFull(x, z); }

// ============================================================================
//  Normal computation
// ============================================================================
void Terrain::computeNormals()
{
    for (auto& v : m_vertices) v.normal = glm::vec3(0.f);
    for (size_t i = 0; i < m_indices.size(); i += 3) {
        GLuint i0 = m_indices[i], i1 = m_indices[i+1], i2 = m_indices[i+2];
        glm::vec3 e1 = m_vertices[i1].position - m_vertices[i0].position;
        glm::vec3 e2 = m_vertices[i2].position - m_vertices[i0].position;
        glm::vec3 fn = glm::cross(e1, e2);
        m_vertices[i0].normal += fn;
        m_vertices[i1].normal += fn;
        m_vertices[i2].normal += fn;
    }
    for (auto& v : m_vertices) {
        float l = glm::length(v.normal);
        v.normal = (l > 1e-5f) ? v.normal / l : glm::vec3(0, 1, 0);
    }
}

// ============================================================================
//  OpenGL buffers for terrain
// ============================================================================
void Terrain::setupBuffers()
{
    // m_buffers is a Buffers RAII object (from Drawable) – handles already
    // created by its constructor.  Do NOT call glGen* again here.
    glBindVertexArray(m_buffers.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_buffers.VBO);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex),
                 m_vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(GLuint),
                 m_indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, texcoord));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, color));  // vec4
    glEnableVertexAttribArray(3);

    glBindVertexArray(0);
}

// ============================================================================
//  Water mesh  –  UV-grid over the ellipse
//
//  We build a regular (gridX × gridZ) quad grid, discard quads whose centre
//  falls outside the ellipse, and place every vertex at WATER_LEVEL.
//  This gives uniform triangulation (no degenerate fan slices near centre)
//  and clean UV coordinates for future animated normal-map or ripple shader.
//
//  Colour: deeper blue at centre, lighter teal at shore; semi-transparent.
// ============================================================================
void Terrain::buildWater()
{
    m_waterVertices.clear();
    m_waterIndices.clear();

    const int gridX = 96;
    const int gridZ = 96;

    // The mesh extends well past the ellipse edge so we can push those
    // outer vertices *down* to the terrain surface, burying the water edge
    // under the shore ramp and making it invisible from any camera angle.
    // overshoot > shoreRel (shoreW/lakeRX ≈ 0.09*halfW / 0.42*halfW ≈ 0.21)
    const float overshoot = 1.50f;
    const float x0 = -m_lakeRX * overshoot, x1 = m_lakeRX * overshoot;
    const float z0 = -m_lakeRZ * overshoot, z1 = m_lakeRZ * overshoot;

    auto vertIndex = [&](int ix, int iz) { return iz * (gridX+1) + ix; };

    m_waterVertices.resize((size_t)(gridX+1) * (gridZ+1));

    for (int iz = 0; iz <= gridZ; ++iz) {
        for (int ix = 0; ix <= gridX; ++ix) {
            float wx = x0 + (x1 - x0) * (float)ix / gridX;
            float wz = z0 + (z1 - z0) * (float)iz / gridZ;

            float ex = wx / m_lakeRX;
            float ez = wz / m_lakeRZ;
            float de = std::sqrt(ex*ex + ez*ez);   // 0=centre, 1=ellipse edge

            // Y position:
            //   inside ellipse (de <= 1)  → flat at WATER_LEVEL
            //   outside (de > 1)          → follows terrain height so the mesh
            //                               dips under the shore and disappears
            float wy;
            if (de <= 1.0f) {
                wy = WATER_LEVEL;
            } else {
                // Snap to terrain; add a tiny offset so it is guaranteed below
                wy = sampleHeightFull(wx, wz) - 0.05f;
                // Never rise above WATER_LEVEL (would poke through the surface)
                wy = std::min(wy, WATER_LEVEL);
            }

            // Colour: deep blue centre → teal shore → invisible outside ellipse
            glm::vec4 deepColor (0.08f, 0.18f, 0.52f, 0.78f);
            glm::vec4 shoreColor(0.18f, 0.55f, 0.76f, 0.42f);
            glm::vec4 edgeColor (0.18f, 0.55f, 0.76f, 0.00f);

            glm::vec4 col;
            if (de <= 1.0f) {
                float ct = de * de;   // eases colour toward shore
                col = glm::mix(deepColor, shoreColor, ct);
            } else {
                float t = glm::clamp((de - 1.0f) / (overshoot - 1.0f), 0.0f, 1.0f);
                // Smooth fade so the buried skirt has no hard alpha edge
                t = t * t * (3.0f - 2.0f * t);
                col = glm::mix(shoreColor, edgeColor, t);
            }

            Vertex& vtx = m_waterVertices[vertIndex(ix, iz)];
            vtx.position = glm::vec3(wx, wy, wz);
            vtx.normal   = glm::vec3(0.f, 1.f, 0.f);
            vtx.texcoord = glm::vec2((float)ix / gridX, (float)iz / gridZ);
            vtx.color    = col;
        }
    }

    // All quads inside the overshoot boundary get indices
    for (int iz = 0; iz < gridZ; ++iz) {
        for (int ix = 0; ix < gridX; ++ix) {
            float cx = x0 + (x1 - x0) * (ix + 0.5f) / gridX;
            float cz = z0 + (z1 - z0) * (iz + 0.5f) / gridZ;
            float ex = cx / m_lakeRX;
            float ez = cz / m_lakeRZ;
            if (ex*ex + ez*ez > overshoot * overshoot) continue;

            GLuint tl = (GLuint)vertIndex(ix,   iz);
            GLuint tr = (GLuint)vertIndex(ix+1, iz);
            GLuint bl = (GLuint)vertIndex(ix,   iz+1);
            GLuint br = (GLuint)vertIndex(ix+1, iz+1);

            m_waterIndices.push_back(tl);
            m_waterIndices.push_back(bl);
            m_waterIndices.push_back(tr);
            m_waterIndices.push_back(tr);
            m_waterIndices.push_back(bl);
            m_waterIndices.push_back(br);
        }
    }
}

// ============================================================================
//  OpenGL buffers for water
// ============================================================================
void Terrain::setupWaterBuffers()
{
    glGenVertexArrays(1, &m_waterBuffers.VAO);
    glGenBuffers(1, &m_waterBuffers.VBO);
    glGenBuffers(1, &m_waterBuffers.EBO);

    glBindVertexArray(m_waterBuffers.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_waterBuffers.VBO);
    glBufferData(GL_ARRAY_BUFFER,
                 m_waterVertices.size() * sizeof(Vertex),
                 m_waterVertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_waterBuffers.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 m_waterIndices.size() * sizeof(GLuint),
                 m_waterIndices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, texcoord));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(3);

    glBindVertexArray(0);
}

// ============================================================================
//  drawWater  –  call AFTER drawing opaque terrain
//
//  Sets object_id = WATER (11) on whichever shader program is currently bound
//  so the fragment shader uses the water branch (translucent, specular).
//  Restores the previous object_id afterwards.
// ============================================================================
void Terrain::drawWater()
{
    // Query the active program so we can set/restore object_id
    GLint prog = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &prog);
    GLint locObjId = (prog > 0) ? glGetUniformLocation(prog, "object_id") : -1;

    // Save current object_id and switch to WATER (11)
    GLint prevId = 0;
    if (locObjId >= 0) {
        glGetUniformiv(prog, locObjId, &prevId);
        glUniform1i(locObjId, 11);   // WATER
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);

    glBindVertexArray(m_waterBuffers.VAO);
    glDrawElements(GL_TRIANGLES,
                   (GLsizei)m_waterIndices.size(),
                   GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);

    // Restore object_id
    if (locObjId >= 0)
        glUniform1i(locObjId, prevId);
}

// ============================================================================
//  draw  –  terrain (opaque) then water (transparent)
// ============================================================================
void Terrain::draw(const DrawContext& /*ctx*/)
{
    glBindVertexArray(m_buffers.VAO);
    glDrawElements(GL_TRIANGLES, (GLsizei)m_indices.size(),
                   GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);

    drawWater();
}

// ============================================================================
//  Queries
// ============================================================================
glm::vec3 Terrain::getNormal(float x, float z) const
{
    float eps = m_spacing * 0.5f;
    float r = sampleHeightFull(x + eps, z);
    float l = sampleHeightFull(x - eps, z);
    float f = sampleHeightFull(x, z + eps);
    float b = sampleHeightFull(x, z - eps);
    return glm::normalize(glm::vec3(l - r, 2.f * eps, b - f));
}