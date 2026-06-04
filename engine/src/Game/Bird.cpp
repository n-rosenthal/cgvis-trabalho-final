#include "Game/Bird.hpp"
#include "Game/BirdDrawable.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

Bird::Bird()
    : GameObject(std::make_unique<BirdDrawable>(),
                 glm::vec3(0.0f, 5.0f, 0.0f), // posição inicial
                 glm::vec3(0.0f, 0.0f, 0.0f), // rotação
                 glm::vec3(1.0f))            // escala
    , rotationY(0.0f), rotationX(0.0f), rotationZ(0.0f)
    , velocity(0.0f), speed(18.0f), moveSpeed(12.0f), rotationSpeed(1.6f)
    , collisionCooldown(0.0f), terrainCollisionCooldown(0.0f)
    , m_collider(glm::vec3(0.0f, -1.5f, 0.0f), glm::vec3(0.0f, 1.5f, 0.0f), 1.0f)
{
    // Gera buffers (vazio)
    if (m_drawable) m_drawable->generate();
}

std::vector<std::shared_ptr<Collider>> Bird::getColliders() {
    std::vector<std::shared_ptr<Collider>> colliders;
    // O Bird possui um colisor capsular, mas a interface Collider é uma classe base.
    // Se CapsuleCollider herda de Collider, você pode retorná-lo.
    colliders.push_back(std::make_shared<CapsuleCollider>(m_collider));
    return colliders;
}

glm::vec3 Bird::getForward() const
{
    glm::mat4 rot(1.0f);

    rot = glm::rotate(rot, rotationY, glm::vec3(0,1,0));
    rot = glm::rotate(rot, rotationX, glm::vec3(1,0,0));
    rot = glm::rotate(rot, rotationZ, glm::vec3(0,0,1));

    glm::vec3 forward = glm::vec3(rot * glm::vec4(1,0,0,0));

    return glm::normalize(forward);
}

glm::vec3 Bird::getUp() const
{
    glm::mat4 rot(1.0f);

    rot = glm::rotate(rot, rotationY, glm::vec3(0,1,0));
    rot = glm::rotate(rot, rotationX, glm::vec3(1,0,0));
    rot = glm::rotate(rot, rotationZ, glm::vec3(0,0,1));

    return glm::normalize(
        glm::vec3(rot * glm::vec4(0,1,0,0))
    );
}

const CapsuleCollider& Bird::getCollider() const {
    return m_collider;
}

void Bird::updateColliders()
{
    glm::vec3 forward = getForward();

    m_collider.p0 =
        m_position - forward * 1.5f;

    m_collider.p1 =
        m_position + forward * 1.5f;
}

void Bird::onCollision(glm::vec3 obstaclePos) {
    if (collisionCooldown > 0.0f) return;
    collisionCooldown = 1.0f;

    glm::vec3 away = glm::normalize(m_position - obstaclePos);
    velocity += away * 20.0f;
    velocity.y += 6.0f;
    speed *= 0.55f;
    if (speed < 2.0f) speed = 2.0f;
    rotationZ += glm::radians((rand() % 2 == 0) ? 60.0f : -60.0f);
    rotationX += glm::radians(-20.0f);
}

bool Bird::onTerrainCollision(float terrainHeight, glm::vec3 terrainNormal) {
    if (terrainCollisionCooldown > 0.0f) return false;
    terrainCollisionCooldown = 0.35f;

    float bodyOffset = 0.6f;
    m_position.y = terrainHeight + bodyOffset;
    float impactSpeed = glm::length(velocity);
    float terrainPitch = atan2(terrainNormal.z, terrainNormal.y);
    float terrainRoll  = -atan2(terrainNormal.x, terrainNormal.y);
    rotationX = glm::mix(rotationX, terrainPitch, 0.35f);
    rotationZ = glm::mix(rotationZ, terrainRoll, 0.35f);

    if (impactSpeed < 8.0f) {
        velocity = glm::reflect(velocity, terrainNormal) * 0.12f;
        speed *= 0.75f;
        return true;
    }
    if (impactSpeed < 18.0f) {
        velocity = glm::reflect(velocity, terrainNormal) * 0.30f;
        velocity.y += 3.0f;
        speed *= 0.45f;
        rotationZ += glm::radians((rand() % 2 == 0) ? 25.0f : -25.0f);
        return true;
    }
    return true;
}

void Bird::update(float dt, GLFWwindow* window)
{
    //--------------------------------------------------
    // INPUT
    //--------------------------------------------------

    float yawInput = 0.0f;
    float pitchInput = 0.0f;
    float throttle = 0.0f;

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        yawInput = 1.0f;

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        yawInput = -1.0f;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        pitchInput = 1.0f;

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        pitchInput = -1.0f;

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        throttle = 1.0f;

    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        throttle = -1.0f;

    //--------------------------------------------------
    // ORIENTAÇÃO
    //--------------------------------------------------

    rotationY += yawInput * rotationSpeed * dt;

    rotationX += pitchInput * rotationSpeed * dt;

    rotationX = glm::clamp(
        rotationX,
        glm::radians(-75.0f),
        glm::radians(75.0f)
    );

    float targetRoll =
        -yawInput * glm::radians(45.0f);

    rotationZ +=
        (targetRoll - rotationZ)
        * 6.0f
        * dt;

    //--------------------------------------------------
    // VELOCIDADE
    //--------------------------------------------------

    speed += throttle * 12.0f * dt;

    float drag = 0.18f;

    speed -= speed * drag * dt;

    if(rotationX > 0.0f)
    {
        speed -= rotationX * 4.0f * dt;
    }
    else
    {
        speed += (-rotationX) * 2.0f * dt;
    }

    speed = glm::clamp(
        speed,
        6.0f,
        40.0f
    );

    //--------------------------------------------------
    // DIREÇÕES
    //--------------------------------------------------

    glm::vec3 forward = getForward();
    glm::vec3 up = getUp();

    //--------------------------------------------------
    // FLAP
    //--------------------------------------------------

    static bool flapPressedLastFrame = false;

    bool flapPressed =
        glfwGetKey(window, GLFW_KEY_SPACE)
        == GLFW_PRESS;

    if (flapPressed && !flapPressedLastFrame)
    {
        velocity += up * 8.0f;
        speed += 2.0f;
    }

    flapPressedLastFrame = flapPressed;

    //--------------------------------------------------
    // VELOCIDADE DESEJADA
    //--------------------------------------------------

    glm::vec3 desiredVelocity =
        forward * speed;

    velocity.x =
        glm::mix(
            velocity.x,
            desiredVelocity.x,
            4.0f * dt
        );

    velocity.z =
        glm::mix(
            velocity.z,
            desiredVelocity.z,
            4.0f * dt
        );

    //--------------------------------------------------
    // SUSTENTAÇÃO
    //--------------------------------------------------

    float lift =
        speed *
        speed *
        0.015f;

    velocity.y += lift * dt;

    //--------------------------------------------------
    // GRAVIDADE
    //--------------------------------------------------

    velocity.y -= 12.0f * dt;

    //--------------------------------------------------
    // AMORTECIMENTO
    //--------------------------------------------------

    velocity *= 0.999f;

    //--------------------------------------------------
    // MOVIMENTO
    //--------------------------------------------------

    m_position += velocity * dt;

    //--------------------------------------------------
    // LIMITES DE ALTURA
    //--------------------------------------------------

    if (m_position.y < -1.0f)
    {
        m_position.y = -1.0f;

        if (velocity.y < 0.0f)
            velocity.y = 0.0f;
    }

    if (m_position.y > 120.0f)
    {
        m_position.y = 120.0f;

        if (velocity.y > 0.0f)
            velocity.y = 0.0f;
    }

    //--------------------------------------------------
    // COLLIDER
    //--------------------------------------------------

    updateColliders();

    //--------------------------------------------------
    // GAMEOBJECT
    //--------------------------------------------------

    m_rotation =
        glm::vec3(
            rotationX,
            rotationY,
            rotationZ
        );

    //--------------------------------------------------
    // COOLDOWNS
    //--------------------------------------------------

    collisionCooldown =
        glm::max(
            0.0f,
            collisionCooldown - dt
        );

    terrainCollisionCooldown =
        glm::max(
            0.0f,
            terrainCollisionCooldown - dt
        );
}