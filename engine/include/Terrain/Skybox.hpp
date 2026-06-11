#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>

// ============================================================================
//  Skybox  –  procedural atmospheric gradient, no external textures needed.
//
//  IMPORTANT – two-phase construction to avoid calling OpenGL before the
//  context is ready (which causes a segfault if the object is a member
//  variable initialized before the GL context exists):
//
//    class MyApp {
//        Skybox m_sky;           // safe: trivial ctor, no GL calls
//        void init() {
//            // ... create window + GL context first ...
//            m_sky.init();       // GL calls happen here
//        }
//    };
//
//  Usage each frame (call BEFORE drawing terrain):
//    m_sky.draw(projection, view, sunDir, timeOfDay);
//
//  timeOfDay: [0,1]  0.0/1.0=midnight  0.25=sunrise  0.5=noon  0.75=sunset
// ============================================================================
class Skybox
{
public:
    Skybox()  = default;   // trivial – no GL calls
    ~Skybox();

    Skybox(const Skybox&)            = delete;
    Skybox& operator=(const Skybox&) = delete;

    // Call once, AFTER the OpenGL context has been created.
    void init();

    // Draw the sky behind all geometry.
    void draw(const glm::mat4& projection,
              const glm::mat4& view,
              const glm::vec3& sunDir,
              float            timeOfDay = 0.5f) const;

    bool isReady() const { return m_shader != 0; }

private:
    GLuint m_VAO    = 0;
    GLuint m_VBO    = 0;
    GLuint m_shader = 0;

    GLint  m_uVP     = -1;
    GLint  m_uSunDir = -1;
    GLint  m_uTOD    = -1;

    void buildCube();
    void buildShader();
    static GLuint compileShader(GLenum type, const char* src);
    static GLuint linkProgram(GLuint vert, GLuint frag);
};