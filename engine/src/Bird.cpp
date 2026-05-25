#include "Bird.hpp"

#include <cmath>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

Bird::Bird()
    : position(0.0f, 5.0f, 0.0f),
      rotationY(0.0f),
      rotationX(0.0f),
      rotationZ(0.0f),
      velocity(0.0f),
      acceleration(0.0f),
      speed(12.0f),
      moveSpeed(12.0f),
      rotationSpeed(2.0f)
{
}

glm::vec3 Bird::getForward() const
{
    glm::vec3 forward;

    forward.x = sin(rotationY);
    forward.y = 0.0f;
    forward.z = cos(rotationY);

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
    // VELOCIDADE
    // =========================================

    float throttle = 0.0f;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        throttle = 1.0f;

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        throttle = -1.0f;

    speed += throttle * 18.0f * dt;

    // drag
    speed *= 0.996f;

    // limites
    speed = glm::clamp(speed, 6.0f, 32.0f);

    // =========================================
    // YAW
    // =========================================

    rotationY += yawInput * rotationSpeed * dt;

    // =========================================
    // ROLL AUTOMÁTICO
    // =========================================

    float targetRoll =
        yawInput * glm::radians(45.0f);

    rotationZ +=
        (targetRoll - rotationZ)
        * 4.0f
        * dt;

    // =========================================
    // DIREÇÃO
    // =========================================

    glm::vec3 forward = getForward();

    velocity.x = forward.x * speed;
    velocity.z = forward.z * speed;

    // =========================================
    // GRAVIDADE
    // =========================================

    velocity.y -= 14.0f * dt;

    // =========================================
    // LIFT
    // =========================================

    float lift =
        speed * 0.55f;

    velocity.y += lift * dt;

    // =========================================
    // FLAP (ESPAÇO)
    // =========================================

    static bool flapPressedLastFrame = false;

    bool flapPressed =
        glfwGetKey(window, GLFW_KEY_SPACE)
        == GLFW_PRESS;

    // impulso apenas no instante do clique
    if (flapPressed && !flapPressedLastFrame)
    {
        velocity.y += 6.5f;

        // pitch instantâneo
        rotationX += glm::radians(10.0f);
    }

    flapPressedLastFrame = flapPressed;

    // =========================================
    // PITCH VISUAL
    // =========================================

    float targetPitch =
        glm::clamp(
            velocity.y * 0.05f,
            glm::radians(-35.0f),
            glm::radians(35.0f)
        );

    rotationX +=
        (targetPitch - rotationX)
        * 3.0f
        * dt;

    // =========================================
    // DAMPING VERTICAL
    // =========================================

    velocity.y *= 0.995f;

    // =========================================
    // MOVIMENTO
    // =========================================

    position += velocity * dt;

    // =========================================
    // CHÃO
    // =========================================

    if (position.y < -1.0f)
    {
        position.y = -1.0f;

        if (velocity.y < 0.0f)
            velocity.y = 0.0f;
    }

    // teto
    if (position.y > 120.0f)
    {
        position.y = 120.0f;

        if (velocity.y > 0.0f)
            velocity.y = 0.0f;
    }
}

void Bird::setModelMatrixUniform(
    GLuint model_uniform,
    const glm::mat4& view,
    const glm::mat4& projection
) const
{
    glm::mat4 model = glm::mat4(1.0f);

    // posição
    model = glm::translate(model, position);

    // correção do modelo OBJ
    model = glm::rotate(
        model,
        glm::radians(90.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    // yaw
    model = glm::rotate(
        model,
        rotationY,
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    // pitch
    model = glm::rotate(
        model,
        rotationX,
        glm::vec3(1.0f, 0.0f, 0.0f)
    );

    // roll
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