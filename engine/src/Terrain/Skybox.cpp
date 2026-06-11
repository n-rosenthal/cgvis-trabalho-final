#include "Terrain/Skybox.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stdexcept>
#include <cstdio>

// ============================================================================
//  Vertex shader
//  Passes the raw object-space position as the view direction to the fragment
//  shader.  The VP matrix has its translation stripped so the cube stays
//  centred on the camera.
// ============================================================================
static const char* VERT_SRC = R"GLSL(
#version 330 core
layout(location = 0) in vec3 aPos;
out vec3 vDir;
uniform mat4 uVP;   // projection * view-without-translation
void main()
{
    vDir = aPos;                              // cube corner = view direction
    vec4 pos = uVP * vec4(aPos, 1.0);
    gl_Position = pos.xyww;                   // force depth = 1.0 (far plane)
}
)GLSL";

// ============================================================================
//  Fragment shader  –  atmospheric procedural sky
//
//  Colour model (simplified Rayleigh + Mie-ish):
//    sky    : zenith (deep blue) → horizon (pale blue/white)
//    sun    : sharp disc + broad halo
//    haze   : warm orange/pink band near horizon at sunrise/sunset
//    night  : dark navy/black when timeOfDay < 0.15 or > 0.85
// ============================================================================
static const char* FRAG_SRC = R"GLSL(
#version 330 core
in  vec3 vDir;
out vec4 fragColor;

uniform vec3  uSunDir;   // normalised, points TOWARD sun
uniform float uTOD;      // [0,1]: 0.5 = noon, 0.25 = sunrise, 0.75 = sunset

// ---- helpers ---------------------------------------------------------------
// Smooth remap
float smoothstep01(float e0, float e1, float x) {
    float t = clamp((x - e0) / (e1 - e0), 0.0, 1.0);
    return t * t * (3.0 - 2.0 * t);
}

// Simple pow-free approximation of Rayleigh scattering intensity
float rayleigh(float cosTheta) {
    return 0.75 * (1.0 + cosTheta * cosTheta);
}

void main()
{
    vec3 dir = normalize(vDir);

    // -- Day/night cycle brightness ------------------------------------------
    // daylight in [0.18, 0.82]; smooth dawn/dusk at edges
    float daylight = smoothstep01(0.15, 0.25, uTOD)
                   * (1.0 - smoothstep01(0.75, 0.85, uTOD));

    // -- Sun parameters -------------------------------------------------------
    float cosS  = dot(dir, normalize(uSunDir));   // [-1, 1]
    float sunElev = uSunDir.y;                    // -1=below, 1=overhead

    // Sun disc (sharp) + inner halo + outer glow
    float disc  = smoothstep01(0.9998, 1.0,    cosS);
    float halo1 = smoothstep01(0.990,  0.9998, cosS) * 0.45;
    float halo2 = smoothstep01(0.970,  0.990,  cosS) * 0.12;
    float sunGlow = disc + halo1 + halo2;

    // -- Sky gradient ---------------------------------------------------------
    float horizon = smoothstep01(-0.08, 0.30, dir.y);  // 0 at horizon, 1 at zenith

    // Daytime colours
    vec3 zenith   = vec3(0.10, 0.32, 0.82);    // deep blue overhead
    vec3 midBlue  = vec3(0.40, 0.65, 0.95);    // lighter mid-sky
    vec3 horizDay = vec3(0.78, 0.88, 0.98);    // pale near horizon
    vec3 skyday   = mix(mix(horizDay, midBlue, horizon * 0.6),
                        zenith, horizon * horizon);

    // Dusk/dawn tint: orange band near horizon when sun is low
    float duskFactor = (1.0 - abs(sunElev)) * (1.0 - abs(sunElev));   // peaks at horizon
    duskFactor *= smoothstep01(0.0, 0.5, daylight);                    // only near sunrise/set
    float horizBand  = smoothstep01(0.15, -0.05, dir.y)               // near-horizon band
                     * smoothstep01(-0.20, 0.0, dir.y);
    vec3 duskColor   = vec3(1.0, 0.45, 0.10);
    skyday += duskColor * duskFactor * horizBand * 0.75;

    // Night colours: dark navy sky, slight blue-grey
    vec3 skynight = vec3(0.01, 0.02, 0.08) + vec3(0.03, 0.04, 0.08) * (1.0 - horizon);

    vec3 sky = mix(skynight, skyday, daylight);

    // -- Sun colour (warm at horizon, white at zenith) -----------------------
    float sunWarmth  = clamp(1.0 - sunElev, 0.0, 1.0);
    vec3  sunColor   = mix(vec3(1.0, 1.0, 0.95),          // white overhead
                           vec3(1.0, 0.55, 0.10),          // orange at horizon
                           sunWarmth * sunWarmth);
    // Dim sun near night
    sunColor *= daylight * 0.98 + 0.02;

    // -- Compose ---------------------------------------------------------------
    vec3 col = sky + sunColor * sunGlow;

    // Slight ground fog: darken below the horizon
    float belowHorizon = smoothstep01(0.0, -0.15, dir.y);
    col = mix(col, vec3(0.55, 0.62, 0.68) * daylight, belowHorizon * 0.6);

    // Stars (simple hash-based for night sky, only above horizon)
    if (daylight < 0.9 && dir.y > 0.0) {
        // Cheap star field via fract noise
        vec3 d = abs(dir);
        float starHash = fract(sin(dot(floor(dir * 180.0),
                                       vec3(127.1, 311.7, 74.3))) * 43758.5);
        float star = step(0.994, starHash) * dir.y * dir.y;
        col += vec3(star) * (1.0 - daylight) * 1.4;
    }

    fragColor = vec4(col, 1.0);
}
)GLSL";

// ============================================================================
//  Skybox construction helpers
// ============================================================================
GLuint Skybox::compileShader(GLenum type, const char* src)
{
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);
    GLint ok = 0;
    glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[1024];
        glGetShaderInfoLog(s, sizeof(log), nullptr, log);
        std::fprintf(stderr, "[Skybox] shader compile error:\n%s\n", log);
        glDeleteShader(s);
        return 0;
    }
    return s;
}

GLuint Skybox::linkProgram(GLuint vert, GLuint frag)
{
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vert);
    glAttachShader(prog, frag);
    glLinkProgram(prog);
    GLint ok = 0;
    glGetProgramiv(prog, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[1024];
        glGetProgramInfoLog(prog, sizeof(log), nullptr, log);
        std::fprintf(stderr, "[Skybox] link error:\n%s\n", log);
        glDeleteProgram(prog);
        return 0;
    }
    return prog;
}

void Skybox::buildShader()
{
    GLuint vert = compileShader(GL_VERTEX_SHADER,   VERT_SRC);
    GLuint frag = compileShader(GL_FRAGMENT_SHADER, FRAG_SRC);
    m_shader = linkProgram(vert, frag);
    glDeleteShader(vert);
    glDeleteShader(frag);

    m_uVP     = glGetUniformLocation(m_shader, "uVP");
    m_uSunDir = glGetUniformLocation(m_shader, "uSunDir");
    m_uTOD    = glGetUniformLocation(m_shader, "uTOD");
}

void Skybox::buildCube()
{
    // Unit cube, inside-facing (winding is intentionally reversed)
    static const float verts[] = {
        -1, -1, -1,  -1, -1,  1,  -1,  1,  1,   -1,  1,  1,  -1,  1, -1,  -1, -1, -1,
         1, -1, -1,   1,  1, -1,   1,  1,  1,    1,  1,  1,   1, -1,  1,   1, -1, -1,
        -1, -1, -1,   1, -1, -1,   1, -1,  1,    1, -1,  1,  -1, -1,  1,  -1, -1, -1,
        -1,  1, -1,  -1,  1,  1,   1,  1,  1,    1,  1,  1,   1,  1, -1,  -1,  1, -1,
        -1, -1,  1,   1, -1,  1,   1,  1,  1,    1,  1,  1,  -1,  1,  1,  -1, -1,  1,
        -1, -1, -1,  -1,  1, -1,   1,  1, -1,    1,  1, -1,   1, -1, -1,  -1, -1, -1,
    };

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

// ============================================================================
//  Constructor / Destructor
// ============================================================================
void Skybox::init()
{
    buildShader();
    buildCube();
}

Skybox::~Skybox()
{
    if (m_VAO)    glDeleteVertexArrays(1, &m_VAO);
    if (m_VBO)    glDeleteBuffers(1, &m_VBO);
    if (m_shader) glDeleteProgram(m_shader);
}

// ============================================================================
//  draw
// ============================================================================
void Skybox::draw(const glm::mat4& projection,
                  const glm::mat4& view,
                  const glm::vec3& sunDir,
                  float            timeOfDay) const
{
    if (!m_shader) return;

    // Strip translation from view so the sky box stays at the camera origin
    glm::mat4 viewNoTranslation = glm::mat4(glm::mat3(view));
    glm::mat4 vp = projection * viewNoTranslation;

    // Depth: draw sky behind everything (LEQUAL so it passes at depth=1)
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);

    glUseProgram(m_shader);
    glUniformMatrix4fv(m_uVP,    1, GL_FALSE, glm::value_ptr(vp));
    glUniform3fv(m_uSunDir, 1, glm::value_ptr(glm::normalize(sunDir)));
    glUniform1f (m_uTOD,    timeOfDay);

    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
}