#include "Bird.hpp"
#include "matrices.h"   // Matrix_Translate, Matrix_Rotate_Y, etc.
#include <cmath>


Bird::Bird()
    : position(0.0f, 0.0f, 0.0f),
      rotationY(0.0f),
      rotationX(0.0f),
      speed(0.0f),
      verticalSpeed(0.0f),
      moveSpeed(5.0f),
      rotationSpeed(2.0f),
      verticalSpeedFactor(3.0f) {}


void Bird::update(float dt, GLFWwindow* window) {
    // Leitura do teclado
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        // Acelera para frente
        speed += moveSpeed * dt;
        if (speed > 15.0f) speed = 15.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        // Freia / reduz velocidade
        speed -= moveSpeed * dt;
        if (speed < 0.0f) speed = 0.0f;
    }
    // Rotação (esquerda/direita)
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        rotationY -= rotationSpeed * dt;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        rotationY += rotationSpeed * dt;
    }
    // Subir/descer (Q/E)
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        verticalSpeed = verticalSpeedFactor;
    } else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        verticalSpeed = -verticalSpeedFactor;
    } else {
        verticalSpeed *= 0.95f; // amortecimento
    }

    // Atualiza posição
    float dx = speed * dt * sin(rotationY);
    float dz = speed * dt * cos(rotationY);
    position.x += dx;
    position.z += dz;
    position.y += verticalSpeed * dt;

    // Limites simples (chão)
    if (position.y < -1.0f) position.y = -1.0f;
    // Limite de altura máxima
    if (position.y > 10.0f) position.y = 10.0f;
}

void Bird::setModelMatrixUniform(GLuint model_uniform, const glm::mat4& view, const glm::mat4& projection) const {
    glm::mat4 model = Matrix_Translate(position.x, position.y, position.z)
                    * Matrix_Rotate_Y(rotationY)
                    * Matrix_Rotate_X(rotationX);
    glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model));
}