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
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <memory>
#include <vector>

/**
*   Estrutura de buffers que devem ser passados aos shaders
*   do OpenGL para renderização em tela.
*/
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

    //  Cópia proibida: dois objetos não podem ownar o mesmo handle
    Buffers(const Buffers&)            = delete;
    Buffers& operator=(const Buffers&) = delete;

    //  Move permitido: transfere ownership, invalida a origem
    Buffers(Buffers&& other) noexcept
        : VAO(other.VAO), VBO(other.VBO), EBO(other.EBO)
    {
        other.VAO = other.VBO = other.EBO = 0;
    }

    Buffers& operator=(Buffers&& other) noexcept {
        if (this != &other) {
            glDeleteVertexArrays(1, &VAO);
            glDeleteBuffers(1, &VBO);
            glDeleteBuffers(1, &EBO);

            VAO = other.VAO;
            VBO = other.VBO;
            EBO = other.EBO;

            other.VAO = other.VBO = other.EBO = 0;
        }
        return *this;
    }
};

/**
*   Estrutura que representa um vértice da malha de triângulos
*   do objeto a ser desenhado
*/
struct Vertex{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texcoord;
    glm::vec3 color;
};

/**
 *  Contexto de renderização, pode variar a depender do shader
 */
struct DrawContext {
    GLint model_uniform;
    GLint object_id_uniform;
    int   object_id;
    // extensível se necessário
};


/**
*   Interface Drawable
*/
class Drawable {
    public:
        virtual ~Drawable() = default;

        virtual void buildMesh()       = 0;
        virtual void computeNormals()  = 0;
        virtual void setupBuffers()    = 0;
        
        
        /**
         *  @brief  Gera a malha de triângulos do objeto (ou lê de arquivo,
         *          conforme implementação), calcula os vetores normais,
         *          set & send os buffers aos shaders OpenGL
         */
        void generate() {
            buildMesh();
            computeNormals();
            setupBuffers();
        };

        /**
         *  @brief  Função de modelagem para o objeto. Cria uma matriz 4x4
         *          para o modelo, aplica translação até sua posição no terreno,
         *          aplica rotações e escala conforme parametrização.
         */
        void model(const DrawContext& ctx) {
            glm::mat4 model(1.0f);

            model = glm::translate(model, m_position);
            model = glm::rotate(model, m_rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::rotate(model, m_rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::rotate(model, m_rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
            model = glm::scale(model, m_scale);
            
            glUniformMatrix4fv(ctx.model_uniform, 1, GL_FALSE, glm::value_ptr(model));
            glUniform1i(ctx.object_id_uniform, ctx.object_id);
            
            draw(ctx);   // cada subclasse implementa isso
        };
        
    protected:
        Buffers              m_buffers;
        std::vector<Vertex>  m_vertices;
        std::vector<GLuint>  m_indices;
        
        virtual void draw(const DrawContext& ctx) = 0;
};