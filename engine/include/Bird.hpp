/** Header para implementação da Ave controlada pelo usuário
*/

#ifndef BIRD_HPP
#define BIRD_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Bird {
    /** A ave controlada pelo usuário */
public:
    // Construtor
    Bird();

    //  Atualiza o estado da ave
    void update(float dt, GLFWwindow* window);

    // Desenha a ave
    void draw(GLuint model_uniform, GLuint object_id_uniform) const; 

    // Getters
    //  Retorna a posição atual da ave
    glm::vec3 getPosition() const { return position; }

    //  Retorna o angulo Y de inclinação da ave
    float getRotationY() const { return rotationY; }

    //  Retorna se o pássaro está em modo standing (parado)
    bool getStanding() const { return standing; }

    // Setters
    //  Altera a posição da ave
    void setModelMatrixUniform(GLuint model_uniform, const glm::mat4& view, const glm::mat4& projection) const;
    //  Define se o pássaro está em modo standing (parado)
    void setStanding(bool standing) { this->standing = standing; }

    // Tamanho do pássaro
    glm::vec3 getSize() const { return size; }
    void setSize(const glm::vec3& s) { size = s; }

private:
    glm::vec3 position;      // X, Y, Z
    float rotationY;         // ângulo em radianos (para onde olha)
    float rotationX;         // inclinação (pitch)
    float speed;             // velocidade atual
    float verticalSpeed;     // velocidade de subida/descida
    glm::vec3 size;          // Tamanho do pássaro (escala)

    // Parâmetros de controle
    float moveSpeed;
    float rotationSpeed;
    float verticalSpeedFactor;
    bool standing;  // Flag para indicar se usa modelo standing (bird2) ou voando (bird)
};

#endif
