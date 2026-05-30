#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

/**
 * @brief Vértice de uma rocha gerada proceduralmente.
 * Um cubo de 8 vértices é truncado em $8$, $16$ ou $24$ `RockVertices`
 * que são projetados sobre uma esfera. É introduzido ruído aleatório
 * para simular variações.
 * 
 * @param position (glm::vec3)
 *      posição local do vértice no cubo
 * @param normal (glm::vec3)
 *      vetor normal do vértice no cubo
 * @param texcoords (glm::vec2)
 *      coordenadas de textura do vértice
 */
struct RockVertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texcoords;
};

/**
 * @brief Rocha gerada proceduralmente a partir de um cubo
 */
class ProceduralRock {
public:
    /**
     * @brief Construtor padrão para rochas geradas proceduralmente
     * 
     * @param pos (glm::vec3)
     *          posição da rocha na cena virtual
     * @param scale (float)
     *          escala da rocha
     * @param vertices (int)
     *          quantidade de vértices da rocha
     */
    ProceduralRock(glm::vec3 pos, float scale, int subdivisions = 2);

    //  Destrutor
    ~ProceduralRock();

    //  Renderizador
    void Draw(GLuint model_uniform);

    // =====================================================
    //  Getters
    // =====================================================
    //  Acessador para a posição da rocha
    glm::vec3 getPosition() const { return position; }

    //  Acessador para o raio da rocha
    float getCollisionRadius() const { return collisionRadius; }

    //  Colisão: retorna true se o ponto estiver dentro do rocha
    bool checkCollision(glm::vec3 point, float radius);

private:
    // =====================================================
    // Geração da geometria
    // =====================================================
    //  produz a icosfera base para a rocha
    void generateSphere(int subdivisions);

    //  a partir do icosfera, produz uma rocha
    void generate();

    //  calcula as normais das faces da rocha
    void computeNormals();

    // =====================================================
    // OpenGL
    // =====================================================
    //  configura os buffers OpenGL
    void setupBuffers();

private:
    //  OpenGL, variáveis de estado
    GLuint VAO = 0;
    GLuint VBO = 0;
    GLuint EBO = 0;

    //  posição da rocha no mundo
    glm::vec3 position;

    //  escala da rocha
    float scale;
    
    //  raio de colisão
    float collisionRadius;

    //  matriz de modelagem
    glm::mat4 model = glm::mat4(1.0f);

    //  flag booleana que indica se há colisão atualmente com a rocha
    bool collided = false;

    //  geometria: índice de vértices (posição, normal e coordenada de textura)
    std::vector<RockVertex> vertices;

    //  geometria: índices
    std::vector<unsigned int> indices;
};