/** Header para implementação da Ave controlada pelo usuário
*/

#ifndef BIRD_HPP
#define BIRD_HPP

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

class Bird {
    /** A ave controlada pelo usuário */
public:
    // Construtor
    Bird();

    //  Atualiza o estado da ave
    void update(float dt, GLFWwindow* window);

    // Desenha a ave
    void draw(GLuint model_uniform) const; 

    // Getters
    //  Retorna a posição atual da ave
    glm::vec3 getPosition() const { return position; }

    //  Retorna o angulo Y de inclinação da ave
    float getRotationY() const { return rotationY; }

    // Setters
    //  Altera a posição da ave
    void setModelMatrixUniform(GLuint model_uniform, const glm::mat4& view, const glm::mat4& projection) const;

private:
    glm::vec3 position;      // X, Y, Z
    float rotationY;         // ângulo em radianos (para onde olha)
    float rotationX;         // inclinação (pitch)
    float speed;             // velocidade atual
    float verticalSpeed;     // velocidade de subida/descida

    // Parâmetros de controle
    float moveSpeed;
    float rotationSpeed;
    float verticalSpeedFactor;
};

#endif
