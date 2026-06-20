#include "Game/Bird.hpp"

#include "Game/Window.hpp" //   variáveis de depuração do pássaro no menu de debug

#include <glm/gtc/matrix_transform.hpp>

namespace {
    // Parâmetros do andar
    static constexpr float WALK_SPEED = 1.3f;
    static constexpr float BOB_AMPLITUDE = 0.15f;
    static constexpr float BOB_AMPLITUDE_LATERAL = 0.06f;   // balanço lateral
    static constexpr float BOB_FREQUENCY = 4.0f;            // Hz
    static constexpr float kFlapImpulseForward = 2.5f;      // impulso para frente ao decolar

    static constexpr float FLAP_IMPULSE_VERTICAL = 12.0f;   // impulso para cima (era 9)
    static constexpr float FLAP_IMPULSE_FORWARD  = 3.0f;    // no standing
    static constexpr float FLAP_SPEED_BOOST     = 3.0f;     // aumento de velocidade ao bater asas
    static constexpr float ACCELERATION         = 8.0f;     // aceleração com W/S
    static constexpr float SPEED_MIN           = 1.0f;
    static constexpr float SPEED_MAX           = 20.0f;

    constexpr float kYawSpeed   = 1.6f;
    constexpr float kPitchSpeed = 1.4f;

    constexpr float kPitchMax =
        glm::radians(70.0f);

    constexpr float kRollTarget =
        glm::radians(40.0f);

    constexpr float kRollSmooth = 5.0f;

    constexpr float kThrottle = 10.0f;
    constexpr float kDrag     = 0.20f;

    constexpr float kLiftCoeff = 0.012f;
    constexpr float kGravity   = 12.0f;

    constexpr float kFlapImpulse = 15.0f;

    constexpr float kVelBlendXZ = 5.0f;
    constexpr float kVelBlendY  = 2.5f;

    constexpr float kSpeedMin = 5.0f;
    constexpr float kSpeedMax = 42.0f;

    constexpr float kHeightMin = -2.0f;
    constexpr float kHeightMax = 120.0f;
}

/**
 * @brief   Construtor padrão para `Bird`
 * 
 */
Bird::Bird() :
    //  Pássaro enquanto objeto do jogo
    GameObject(
        //  Desenhável do pássaro
        std::make_unique<ObjDrawable>(
            Assets::BIRD_STANDING_MODEL
        ),
        //  Posição inicial
        glm::vec3(
            -345.0f,
            50.0f,
            -5.0f
        ),
        //  Velocidade inicial
        glm::vec3(0.0f),

        //  Escala padrão
        glm::vec3(DEFAULT_SIZE)
    ),

    //  Colisor do pássaro
    m_collider(
        //  Pontos extremos
        glm::vec3(
            0.0f,
            -1.5f,
            0.0f
        ),
        glm::vec3(
            0.0f,
            1.5f,
            0.0f
        ),
        1.0f )
{
    //  Inicializar o pássaro utilizando o modelo padrão (voando)
    m_currentModel =
        &Assets::BIRD_STANDING_MODEL;
}

/**
 * @brief   Matriz de rotação do pássaro
 * @details Composição: yaw → pitch → roll
 * @return  glm::mat4 
 */
glm::mat4 Bird::rotationMatrix() const {
    glm::mat4 Ry = Matrix_Rotate_Y(m_yaw);
    glm::mat4 Rx = Matrix_Rotate_X(m_pitch);
    glm::mat4 Rz = Matrix_Rotate_Z(m_roll);
    return Rz * Rx * Ry;   // yaw → pitch → roll
}

/**
 * @brief       Direção para frente
 * @details     u := (v, 0.0f) => u / |u|
 * 
 * @return      glm::vec3 
 */
glm::vec3 Bird::getForward() const {
    return norm3(glm::vec3(rotationMatrix() * glm::vec4(-1, 0, 0, 0)));
}

/**
 * @brief       Direção para cima
 * @details     u := (v, 0.0f) => u / |u|
 * @return      glm::vec3 
 */
glm::vec3 Bird::getUp() const {
    return norm3(glm::vec3(rotationMatrix() * glm::vec4(0, 1, 0, 0)));
}

/**
 * @brief   Acessador ao colisor do pássaro
 * 
 * @return const CapsuleCollider& 
 */
const CapsuleCollider& Bird::getCollider() const { return m_collider; };

/**
 * @brief   Retorna os colisores do pássaro
 * 
 * @return std::vector<std::shared_ptr<Collider>> 
 */
std::vector<std::shared_ptr<Collider>> Bird::getColliders() {
    return {std::make_shared<CapsuleCollider>(m_collider)};
}

/**
 * @brief   Atualiza o modelo do pássaro conforme o estado atual
 * 
 */
void Bird::updateDrawable() {
    //  Decide entre modelo voando ou modelo parado
    const ModelDefinition* desired = standing
            ? &Assets::BIRD_STANDING_MODEL
            : &Assets::BIRD_MODEL;

    //  se o modelo desejado já é o atual, retorna
    if(desired == m_currentModel)
        return;


    //  alteração do modelo
    m_currentModel = desired;
    m_drawable = std::make_unique<ObjDrawable>(*desired);
}

/**
 * @brief   Tratamento de colisão do pássaro contra o chão
 * 
 * @param terrainHeight 
 * @param terrainNormal 
 * @return true 
 * @return false 
 */
bool Bird::onTerrainCollision(float terrainHeight, glm::vec3 terrainNormal) {
    if (m_velocity.y <= 0.5f) {
        standing = true;
        m_velocity = glm::vec3(0.0f);
        m_position.y = terrainHeight + 0.6f;
        return true;
    }
    return false;
}

/**
 * @brief  Função nula pois os outros tipos de colisão são atualmente
 *          resolvidos em Scene.cpp
 * 
 * @param obstaclePos 
 */
void Bird::onCollision(glm::vec3 obstaclePos) {};

/**
 * @brief   Atualizador dinâmico para o pássaro
 * 
 * @param dt 
 * @param window 
 */
void Bird::update(float dt, GLFWwindow* window, float terrainHeight, glm::vec3 terrainNormal) {
    // Depuração
    g_DebugBirdPosition = m_position;
    g_DebugBirdVelocity = m_velocity;
    g_DebugBirdSpeed = m_speed;
    g_DebugBirdRotation = m_rotation;

    // ==========================================
    //  ESTADO: ANDANDO (standing)
    // ==========================================
    if (standing) {
        // --- Controles de movimento no chão ---
        float yawInput = 0.0f;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) yawInput += 1.0f;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) yawInput -= 1.0f;

        float forwardInput = 0.0f;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) forwardInput += 1.0f;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) forwardInput -= 1.0f;

        // Aplica rotação (yaw)
        m_yaw += yawInput * kYawSpeed * dt;

        // Movimento (velocidade limitada)
        glm::vec3 forward = getForward();
        float speed = forwardInput * WALK_SPEED;
        m_position += forward * speed * dt;

        // --- Animação de "bob" vertical e rotação lateral (roll) ---
        m_walkTime += dt;
        float bobVertical   = BOB_AMPLITUDE * sin(BOB_FREQUENCY * m_walkTime * 2.0f * M_PI);
        float bobRoll       = BOB_AMPLITUDE_LATERAL * sin(BOB_FREQUENCY * m_walkTime * 2.0f * M_PI);

        // Posição vertical
        m_baseY = terrainHeight + 0.5f;
        m_position.y = m_baseY + bobVertical;

        // Pitch sempre zero no chão
        m_pitch = 0.0f;
        // Roll oscilante (balanço lateral)
        m_roll = bobRoll;

        // Zera velocidade (evita acúmulo de inércia do voo)
        m_velocity = glm::vec3(0.0f);

        // Pitch zerado no chão
        m_pitch = 0.0f;

        // --- Decolar com ESPAÇO ---
        bool flapPressed = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
        if (flapPressed && !m_flapHeld) {
            standing = false;
            m_velocity.y = FLAP_IMPULSE_VERTICAL;
            m_velocity += forward * FLAP_IMPULSE_FORWARD;
            m_flapHeld = true;
        }

        // --- Aplica rotação final (com offset para alinhar o modelo) ---
        const float MODEL_OFFSET = -M_PI / 2.0f;   // como estava antes
        m_rotation = glm::vec3(0.0f, m_yaw + MODEL_OFFSET, 0.0f); // standing
        setRotation(m_rotation);

        updateDrawable();
        updateColliders();
        return;
    }

    // ==========================================
    //  ESTADO: VOANDO
    // ==========================================
    // --- Flap (SPACE) durante o voo ---
    bool flapPressed = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
    if (flapPressed && !m_flapHeld) {
        m_velocity.y += FLAP_IMPULSE_VERTICAL;
        m_speed += 2.0f;  // acelera um pouco
        m_flapHeld = true;
    }
    m_flapHeld = flapPressed;

    // --- Aceleração (W/S) ---
    float speedInput = 0.0f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) speedInput += 1.0f;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) speedInput -= 1.0f;

    m_speed += speedInput * ACCELERATION * dt;
    m_speed = glm::clamp(m_speed, SPEED_MIN, SPEED_MAX);

    // --- Yaw (A/D) ---
    float yawInput = 0.0f;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) yawInput += 1.0f;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) yawInput -= 1.0f;
    m_yaw += yawInput * kYawSpeed * dt;

    // --- Pitch automático (baseado na velocidade) ---
    float verticalSpeed   = m_velocity.y;
    float horizontalSpeed = glm::length(glm::vec2(m_velocity.x, m_velocity.z));

    if (horizontalSpeed > 0.1f) {
        float targetPitch = atan2(verticalSpeed, horizontalSpeed);
        m_pitch = glm::mix(m_pitch, targetPitch, 0.05f);
    } else {
        // Se estiver parado horizontalmente, tende a zero
        m_pitch = glm::mix(m_pitch, 0.0f, 0.05f);
    }
    m_pitch = glm::clamp(m_pitch, -kPitchMax, kPitchMax);

    // --- Roll (curva) ---
    float rollTarget = -yawInput * kRollTarget;
    m_roll += (rollTarget - m_roll) * kRollSmooth * dt;

    // --- Física ---
    glm::vec3 forward = getForward();
    glm::vec3 desired = forward * m_speed;

    m_velocity.x = glm::mix(m_velocity.x, desired.x, kVelBlendXZ * dt);
    m_velocity.z = glm::mix(m_velocity.z, desired.z, kVelBlendXZ * dt);
    m_velocity.y = glm::mix(m_velocity.y, desired.y, kVelBlendY * dt);
    m_velocity.y += m_speed * m_speed * kLiftCoeff * dt;
    m_velocity.y -= kGravity * dt;

    m_position += m_velocity * dt;
    clampPosition();

    m_baseY = m_position.y;

    // --- Aplica rotação final (sem offset) ---
    m_rotation = glm::vec3(m_pitch, m_yaw, m_roll);
    setRotation(m_rotation);

    updateDrawable();
    updateColliders();
};

/**
 * @brief   Atualizador dinâmico para os colisores
 */
void Bird::updateColliders(){
    glm::vec3 fwd = getForward();
    m_collider.p0 = m_position
                    -   fwd 
                    *   1.5f;

    m_collider.p1 = m_position
                    +   fwd
                    *   1.5f;
};

/**
 * @brief   Limites de altura para o pássaro
 * 
 */
void Bird::clampPosition() {
    //  Limite inferior
    if(m_position.y < kHeightMin) { m_position.y = kHeightMin; };

    //  Limite superior
    if(m_position.y > kHeightMax) { m_position.y = kHeightMax; };
};
