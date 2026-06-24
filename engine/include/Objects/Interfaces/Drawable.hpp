/**
*   Interface `Drawable`
*   Um objeto que implementa a interface `Drawable` é um objeto que
*   possui os campos e os métodos necessários para ser renderizado
*   na cena virtual.
*
*   1.  campo `buffers` (struct `Buffers`) para armazenar buffers OpenGL
*   2.  campo `vertices` (vetor de structs `Vertex`) para armazenar os vértices da malha de triângulos do objeto
*   3.  campo `indices` (vetor de índices aos triângulos)
*   4.  método  `buildMesh()`: geração da malha de triângulos
*   5.  método  `computeNormals()`: computação dos vetores normais para todos os vértices
*   6.  método  `setupBuffers()`: setting e envio dos buffers aos shaders OpenGL
*   7.  método `generate()`: wrapper que invoca os métodos [4, 6] sequencialmente
*   8.  método `model()`: modelagem, idêntica a todos os objetos
*   9.  método `draw()`: único a cada objeto
*/
#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "matrices.h"

#include <memory>
#include <vector>

// ============================================================================
//  Buffers  –  RAII wrapper for VAO/VBO/EBO
//
//  IMPORTANT: the constructor calls glGen*, so this struct (and any class
//  that contains it as a member) must only be constructed AFTER a valid
//  OpenGL context exists.  Never declare a Drawable subclass as a global or
//  as a class member that is default-constructed before gladLoadGL().
// ============================================================================
struct Buffers {
    GLuint VAO = 0;
    GLuint VBO = 0;
    GLuint EBO = 0;

    Buffers() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
    }

    ~Buffers() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }

    Buffers(const Buffers&)            = delete;
    Buffers& operator=(const Buffers&) = delete;

    Buffers(Buffers&& other) noexcept
        : VAO(other.VAO), VBO(other.VBO), EBO(other.EBO)
    { other.VAO = other.VBO = other.EBO = 0; }

    Buffers& operator=(Buffers&& other) noexcept {
        if (this != &other) {
            glDeleteVertexArrays(1, &VAO);
            glDeleteBuffers(1, &VBO);
            glDeleteBuffers(1, &EBO);
            VAO = other.VAO; VBO = other.VBO; EBO = other.EBO;
            other.VAO = other.VBO = other.EBO = 0;
        }
        return *this;
    }
};

// ============================================================================
//  Vertex
//
//  color = vec4: rgb + alpha.  alpha é 1.0 para todos objetos opacos;
//                              exceto água e possíveis objetos transparentes
// ============================================================================
struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texcoord;
    glm::vec4 color;     // vec3 – alpha channel added for water transparency
};

// ============================================================================
//  DrawContext
// ============================================================================
struct DrawContext {
    GLuint shader_program;
    GLint  model_uniform;
    GLint  view_uniform;
    GLint  projection_uniform;
    GLint  object_id_uniform;
    GLint  diffuse_texture_uniform;
    GLint  color_uniform;           // <-- novo
    int    object_id;
    glm::vec4 color;                // <-- novo, para cor sólida (opcional)
};

// ============================================================================
//  Drawable interface
// ============================================================================
class Drawable {
public:
    virtual ~Drawable() = default;

    virtual void buildMesh()      = 0;
    virtual void computeNormals() = 0;
    virtual void setupBuffers()   = 0;

    void generate() {
        buildMesh();
        computeNormals();
        setupBuffers();
    }

    virtual void model(const DrawContext& ctx) {
        glm::mat4 T = Matrix_Translate(
            m_position.x,
            m_position.y,
            m_position.z
        );

        glm::mat4 Ry = Matrix_Rotate_Y(m_rotation.y);
        glm::mat4 Rx = Matrix_Rotate_X(m_rotation.x);
        glm::mat4 Rz = Matrix_Rotate_Z(m_rotation.z);

        glm::mat4 S = Matrix_Scale(
            m_scale.x,
            m_scale.y,
            m_scale.z
        );

        glm::mat4 m = T * Ry * Rx * Rz * S;

        glUniformMatrix4fv(
            ctx.model_uniform,
            1,
            GL_FALSE,
            glm::value_ptr(m)
        );

        glUniform1i(
            ctx.object_id_uniform,
            ctx.object_id
        );

        draw(ctx);
    }


    glm::vec3 getPosition() const { return m_position; }
    glm::vec3 getRotation() const { return m_rotation; }
    glm::vec3 getScale()    const { return m_scale; }

    void setPosition(const glm::vec3& p) { m_position = p; }
    void setRotation(const glm::vec3& r) { m_rotation = r; }
    void setScale   (const glm::vec3& s) { m_scale    = s; }

protected:
    glm::vec3 m_position = glm::vec3(0.0f);
    glm::vec3 m_rotation = glm::vec3(0.0f);
    glm::vec3 m_scale    = glm::vec3(1.0f);

    Buffers             m_buffers;
    std::vector<Vertex> m_vertices;
    std::vector<GLuint> m_indices;
    DrawContext         m_ctx;

    virtual void draw(const DrawContext& ctx) = 0;
};