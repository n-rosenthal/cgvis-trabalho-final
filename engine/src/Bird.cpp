
#include "Bird.hpp"
#include <cmath>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

Bird::Bird()
    : position(0.0f, 0.0f, 0.0f),
        rotationY(0.0f),
        rotationX(0.0f),
        rotationZ(0.0f),
        velocity(0.0f),
        acceleration(0.0f),
        speed(0.0f),
        moveSpeed(5.0f),
        rotationSpeed(2.0f) {}

glm::vec3 Bird::getForward() const
{
    glm::vec3 forward;

    forward.x = sin(rotationY) * cos(rotationX);
    forward.y = sin(rotationX);
    forward.z = cos(rotationY) * cos(rotationX);

    return glm::normalize(forward);
}

void Bird::update(float dt, GLFWwindow* window)
{
    // =========================================
    // INPUT
    // =========================================

    float yawInput = 0.0f;

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        yawInput = 1.0f;

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        yawInput = -1.0f;

    // =========================================
    // ACELERAÇÃO / FREIO
    // =========================================

    float throttle = 0.0f;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        throttle = 1.0f;

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        throttle = -1.0f;

    speed += throttle * 12.0f * dt;

    // drag aerodinâmico
    speed *= 0.992f;

    // limites
    if (speed < 4.0f)
        speed = 4.0f;

    if (speed > 30.0f)
        speed = 30.0f;

    // =========================================
    // YAW
    // =========================================

    rotationY += yawInput * rotationSpeed * dt;

    // =========================================
    // ROLL AUTOMÁTICO (banking)
    // =========================================

    float targetRoll =
        yawInput * glm::radians(40.0f);

    rotationZ +=
        (targetRoll - rotationZ)
        * 3.5f
        * dt;

    // =========================================
    // FORWARD
    // =========================================

    glm::vec3 forward = getForward();

    // =========================================
    // VELOCIDADE HORIZONTAL
    // =========================================

    velocity.x = forward.x * speed;
    velocity.z = forward.z * speed;

    // =========================================
    // GRAVIDADE
    // =========================================

    velocity.y -= 9.81f * 0.45f * dt;

    // =========================================
    // LIFT (sustentação)
    // =========================================

    float lift =
        speed * speed * 0.018f;

    velocity.y += lift * dt;

    // =========================================
    // SUBIDA AUTOMÁTICA EM CURVAS
    // =========================================

    velocity.y +=
        abs(rotationZ)
        * 1.2f
        * dt;

    // =========================================
    // PITCH VISUAL
    // =========================================

    float targetPitch =
        glm::clamp(
            velocity.y * 0.08f,
            glm::radians(-25.0f),
            glm::radians(25.0f)
        );

    rotationX +=
        (targetPitch - rotationX)
        * 2.5f
        * dt;

    // =========================================
    // MOVIMENTO
    // =========================================

    position += velocity * dt;

    // =========================================
    // LIMITES
    // =========================================

    if (position.y < -1.0f)
    {
        position.y = -1.0f;

        if (velocity.y < 0.0f)
            velocity.y = 0.0f;
    }

    if (position.y > 80.0f)
    {
        position.y = 80.0f;
    }
}

void Bird::setModelMatrixUniform(
    GLuint model_uniform,
    const glm::mat4& view,
    const glm::mat4& projection
) const
{
    glm::mat4 model = glm::mat4(1.0f);

    // posição do pássaro
    model = glm::translate(model, position);

    // Corração da orientação do pássaro
    // model = glm::rotate(model, rotationY, glm::vec3(0,1,0));
    // model = glm::rotate(model, rotationX, glm::vec3(1,0,0));
    // model = glm::rotate(model, rotationZ, glm::vec3(0,0,1));
    model = glm::rotate(
        model,
        glm::radians(90.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );
    
    // yaw (virar esquerda/direita)
    model = glm::rotate(
        model,
        rotationY,
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    // pitch (subir/descer)
    model = glm::rotate(
        model,
        rotationX,
        glm::vec3(1.0f, 0.0f, 0.0f)
    );

    // roll (inclinação nas curvas)
    model = glm::rotate(
        model,
        rotationZ,
        glm::vec3(0.0f, 0.0f, 1.0f)
    );

    glUniformMatrix4fv(
        model_uniform,
        1,
        GL_FALSE,
        glm::value_ptr(model)
    );
}