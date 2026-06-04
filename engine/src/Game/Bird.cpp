#include "Game/Bird.hpp"
#include "Game/BirdDrawable.hpp"
#include <glm/gtc/matrix_transform.hpp>

// ── Constantes de tuning (fáceis de ajustar) ─────────────────────────────────
namespace {
    constexpr float kYawSpeed      = 1.6f;   // rad/s yaw
    constexpr float kPitchSpeed    = 1.4f;   // rad/s pitch
    constexpr float kPitchMax      = glm::radians(70.0f);
    constexpr float kRollTarget    = glm::radians(40.0f); // roll ao virar
    constexpr float kRollSmooth    = 5.0f;   // suavização do roll

    constexpr float kThrottle      = 10.0f;  // aceleração por shift/ctrl
    constexpr float kDrag          = 0.20f;  // atrito aerodinâmico
    constexpr float kLiftCoeff     = 0.012f; // sustentação ∝ speed²
    constexpr float kGravity       = 12.0f;
    constexpr float kFlapImpulse   = 9.0f;
    constexpr float kVelBlendXZ    = 5.0f;   // quão rápido XZ segue forward
    constexpr float kVelBlendY     = 2.5f;

    constexpr float kSpeedMin      = 5.0f;
    constexpr float kSpeedMax      = 42.0f;
    constexpr float kHeightMin     = -1.0f;
    constexpr float kHeightMax     = 120.0f;
}
// ─────────────────────────────────────────────────────────────────────────────

Bird::Bird()
    : GameObject(std::make_unique<BirdDrawable>(),
                 glm::vec3(0.0f, 5.0f, 0.0f),
                 glm::vec3(0.0f),
                 glm::vec3(1.0f))
    , m_collider(glm::vec3(0.0f, -1.5f, 0.0f), glm::vec3(0.0f, 1.5f, 0.0f), 1.0f)
{
    if (m_drawable) m_drawable->generate();
}

// ── Helpers ───────────────────────────────────────────────────────────────────

glm::mat4 Bird::rotationMatrix() const
{
    glm::mat4 r(1.0f);
    r = glm::rotate(r, m_yaw,   glm::vec3(0, 1, 0));
    r = glm::rotate(r, m_pitch, glm::vec3(1, 0, 0));
    r = glm::rotate(r, m_roll,  glm::vec3(0, 0, 1));
    return r;
}

glm::vec3 Bird::getForward() const
{
    return glm::normalize(glm::vec3(rotationMatrix() * glm::vec4(1, 0, 0, 0)));
}

glm::vec3 Bird::getUp() const
{
    return glm::normalize(glm::vec3(rotationMatrix() * glm::vec4(0, 1, 0, 0)));
}

const CapsuleCollider& Bird::getCollider() const { return m_collider; }

std::vector<std::shared_ptr<Collider>> Bird::getColliders()
{
    return { std::make_shared<CapsuleCollider>(m_collider) };
}

// ── Colisões ──────────────────────────────────────────────────────────────────

void Bird::onCollision(glm::vec3 obstaclePos)
{
    if (m_hitCooldown > 0.0f) return;
    m_hitCooldown = 1.0f;

    glm::vec3 away = glm::normalize(m_position - obstaclePos);
    m_velocity += away * 18.0f;
    m_velocity.y += 5.0f;
    m_speed     *= 0.5f;
    m_roll      += glm::radians((rand() % 2 == 0) ? 55.0f : -55.0f);
    m_pitch     += glm::radians(-18.0f);
}

bool Bird::onTerrainCollision(float terrainHeight, glm::vec3 terrainNormal)
{
    if (m_terrainCooldown > 0.0f) return false;
    m_terrainCooldown = 0.35f;

    m_position.y = terrainHeight + 0.6f;

    // Alinha gradualmente com a normal do terreno
    m_pitch = static_cast<float>(glm::mix(static_cast<float>(m_pitch), static_cast<float>(atan2(terrainNormal.z, terrainNormal.y)), static_cast<float>(0.35f)));
    m_roll  = static_cast<float>(glm::mix(static_cast<float>(m_roll), static_cast<float>(-atan2(terrainNormal.x, terrainNormal.y)), 0.35f));

    float impact = glm::length(m_velocity);
    float bounce = (impact < 8.0f) ? 0.10f : 0.28f;

    m_velocity = glm::reflect(m_velocity, terrainNormal) * bounce;
    if (impact >= 8.0f) {
        m_velocity.y += 2.5f;
        m_speed      *= 0.45f;
        m_roll       += glm::radians((rand() % 2 == 0) ? 22.0f : -22.0f);
    } else {
        m_speed *= 0.75f;
    }
    return true;
}

// ── Update ────────────────────────────────────────────────────────────────────

void Bird::update(float dt, GLFWwindow* window)
{
    //-- Leitura de input -------------------------------------------------------
    const float yawInput   = (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS ?  1.0f : 0.0f)
                           + (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS ? -1.0f : 0.0f);

    const float pitchInput = (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS ?  1.0f : 0.0f)
                           + (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS ? -1.0f : 0.0f);

    const float throttle   = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)   == GLFW_PRESS ?  1.0f : 0.0f)
                           + (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS ? -1.0f : 0.0f);

    const bool flapPressed = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;

    //-- Orientação -------------------------------------------------------------
    m_yaw   += yawInput   * kYawSpeed   * dt;
    m_pitch += pitchInput * kPitchSpeed * dt;
    m_pitch  = glm::clamp(m_pitch, -kPitchMax, kPitchMax);

    // Roll suave proporcional ao yaw input
    const float rollTarget = -yawInput * kRollTarget;
    m_roll += (rollTarget - m_roll) * kRollSmooth * dt;

    //-- Velocidade escalar -----------------------------------------------------
    m_speed += throttle * kThrottle * dt;

    // Arrasto aerodinâmico
    m_speed -= m_speed * kDrag * dt;

    // Subir custa velocidade; descer ganha um pouco
    m_speed -= m_pitch * 3.5f * dt;

    m_speed = glm::clamp(m_speed, kSpeedMin, kSpeedMax);

    //-- Flap (borda de subida) ------------------------------------------------
    if (flapPressed && !m_flapHeld)
        m_velocity.y += kFlapImpulse;

    m_flapHeld = flapPressed;

    //-- Integração de velocidade -----------------------------------------------
    glm::vec3 forward = getForward();
    glm::vec3 desired  = forward * m_speed;

    // Componentes XZ seguem forward rapidamente; Y mais lentamente (inércia)
    m_velocity.x = glm::mix(m_velocity.x, desired.x, kVelBlendXZ * dt);
    m_velocity.z = glm::mix(m_velocity.z, desired.z, kVelBlendXZ * dt);
    m_velocity.y = glm::mix(m_velocity.y, desired.y, kVelBlendY  * dt);

    // Sustentação ∝ speed²
    const float lift = m_speed * m_speed * kLiftCoeff;
    m_velocity.y += lift    * dt;
    m_velocity.y -= kGravity * dt;

    //-- Posição ----------------------------------------------------------------
    m_position += m_velocity * dt;
    clampPosition();

    //-- Sincroniza estado ------------------------------------------------------
    m_rotation = glm::vec3(m_pitch, m_yaw, m_roll);
    updateColliders();

    m_hitCooldown     = glm::max(0.0f, m_hitCooldown     - dt);
    m_terrainCooldown = glm::max(0.0f, m_terrainCooldown - dt);
}

void Bird::clampPosition()
{
    if (m_position.y < kHeightMin) { m_position.y = kHeightMin; m_velocity.y = glm::max(0.0f, m_velocity.y); }
    if (m_position.y > kHeightMax) { m_position.y = kHeightMax; m_velocity.y = glm::min(0.0f, m_velocity.y); }
}

void Bird::updateColliders()
{
    glm::vec3 fwd = getForward();
    m_collider.p0 = m_position - fwd * 1.5f;
    m_collider.p1 = m_position + fwd * 1.5f;
}