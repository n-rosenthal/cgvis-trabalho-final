#include "Game/Bird.hpp"

#include "Game/Window.hpp" //   variáveis de depuração do pássaro no menu de debug

namespace {
    // Parâmetros do andar
    static constexpr float WALK_SPEED = 1.3f;
    static constexpr float BOB_AMPLITUDE = 0.15f;
    static constexpr float BOB_AMPLITUDE_LATERAL = 0.06f;   // balanço lateral
    static constexpr float BOB_FREQUENCY = 4.0f;            // Hz

    static constexpr float FLAP_IMPULSE_VERTICAL = 12.0f;   // impulso para cima (era 9)
    static constexpr float FLAP_IMPULSE_FORWARD  = 3.0f;    // no standing
    static constexpr float ACCELERATION         = 8.0f;     // aceleração com W/S
    static constexpr float SPEED_MIN           = 1.0f;
    static constexpr float SPEED_MAX           = 20.0f;

    constexpr float kYawSpeed   = 1.6f;

    constexpr float kPitchMax =
        glm::radians(70.0f);

    constexpr float kRollTarget =
        glm::radians(40.0f);

    constexpr float kRollSmooth = 5.0f;

    constexpr float kLiftCoeff = 0.012f;
    constexpr float kGravity   = 12.0f;

    constexpr float kVelBlendXZ = 5.0f;
    constexpr float kVelBlendY  = 2.5f;

    constexpr float kHeightMin = -2.5f;
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
        glm::vec3(0.0f, -1.5f, 0.0f),
        glm::vec3(0.0f, 1.5f, 0.0f),
        1.0f )
{
    //  Ajuste yaw inicial para que o pássaro nasça voltado para a frente do jogador
    m_yaw = glm::radians(180.0f);
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
 * @brief   Matriz de rotação do pássaro, sem o roll de bob do andar
 * @details Enquanto pousado, m_roll é usado apenas para a animação
 *          visual de balanço lateral do modelo (bob); como o roll
 *          entra na composição da matriz (Rz * Rx * Ry), ele desvia
 *          tanto o vetor "forward" quanto o "up" resultantes, não
 *          apenas a orientação visual. Esta versão omite o roll
 *          propositalmente para uso pela câmera, que não deve
 *          tremer/balançar junto com essa animação cosmética.
 *          Em voo, o roll faz parte da física real de curva e a
 *          matriz completa (rotationMatrix()) é usada normalmente.
 * @return  glm::mat4
 */
glm::mat4 Bird::rotationMatrixNoBob() const {
    if (!standing)
        return rotationMatrix();

    glm::mat4 Ry = Matrix_Rotate_Y(m_yaw);
    glm::mat4 Rx = Matrix_Rotate_X(m_pitch);
    // roll omitido propositalmente (bob cosmético)
    return Rx * Ry;
}

/**
 * @brief       Direção para frente
 * @details     u := (v, 0.0f) => u / |u|
 * 
 * @return      glm::vec3 
 */
glm::vec3 Bird::getForward() const {
    return normalize(glm::vec3(rotationMatrix() * glm::vec4(-1, 0, 0, 0)));
}

/**
 * @brief       Direção para frente, sem o roll de bob do andar
 * @details     Ver rotationMatrixNoBob(). Usado pela câmera, já
 *              que Camera::update() calcula a posição da câmera
 *              a partir do forward — um roll oscilante aqui se
 *              traduzia em tremor de posição da câmera inteira,
 *              não só de inclinação.
 * @return      glm::vec3
 */
glm::vec3 Bird::getForwardNoBob() const {
    return normalize(glm::vec3(rotationMatrixNoBob() * glm::vec4(-1, 0, 0, 0)));
}

/**
 * @brief       Direção para cima
 * @details     u := (v, 0.0f) => u / |u|
 * @return      glm::vec3 
 */
glm::vec3 Bird::getUp() const {
    return normalize(glm::vec3(rotationMatrix() * glm::vec4(0, 1, 0, 0)));
}

/**
 * @brief       Direção para cima, sem o roll de bob do andar
 * @details     Ver rotationMatrixNoBob().
 * @return      glm::vec3
 */
glm::vec3 Bird::getUpNoBob() const {
    return normalize(glm::vec3(rotationMatrixNoBob() * glm::vec4(0, 1, 0, 0)));
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
std::vector<std::shared_ptr<Collider>> Bird::getColliders() const {
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
    // Se o pássaro já está colado ao chão (standing), não faz nada
    if (standing) {
        // Apenas mantém a posição ajustada ao terreno (caso o terreno mude)
        m_position.y = terrainHeight + 0.6f;
        return false; // não precisa de som
    }

    // Verifica se está abaixo do terreno (colisão)
    float birdBottom = m_position.y - 0.6f; // assume que o centro está 0.6 acima dos pés
    if (birdBottom <= terrainHeight) {
        // Coloca exatamente em cima
        float targetY = terrainHeight + 0.6f;
        m_position.y = glm::mix(m_position.y, targetY, 0.5f);
        
        // Zera a velocidade vertical e marca como pousado
        m_velocity.y = 0.0f;
        standing = true;
        
        // Se estiver descendo, para toda velocidade
        if (m_velocity.y < 0.0f) {
            m_velocity = glm::vec3(0.0f);
        }
        
        return true; // houve colisão
    }
    
    return false;
}

/**
 * @brief  Função nula pois os outros tipos de colisão são atualmente
 *          resolvidos em Scene.cpp
 * 
 * @param obstaclePos 
 */
void Bird::onCollision(glm::vec3 obstaclePos) {}

/**
 * @brief   Resposta "suave" de colisão, usada por obstáculos
 *          atravessáveis (ex: arbustos): em vez de barrar o
 *          movimento como onCollision(), desacelera o pássaro
 *          e o empurra lateralmente, simulando atrito/atravessar
 *          a vegetação. O som correspondente é tocado por quem
 *          chama este método (ver Scene::resolveCollisions()).
 * 
 * @param   obstaclePos (glm::vec3)
 *          posição do obstáculo atravessável
 */
void Bird::onBushCollision(glm::vec3 obstaclePos) {
    constexpr float kSlowFactor   = 0.5f;  // reduz a velocidade pela metade
    constexpr float kPushStrength = 2.0f;  // intensidade do empurrão lateral

    // Desacelera
    m_speed     *= kSlowFactor;
    m_velocity  *= kSlowFactor;

    // Empurra para longe do arbusto, no plano horizontal
    glm::vec3 away = m_position - obstaclePos;
    away.y = 0.0f;

    if (glm::length(away) > 0.0001f) {
        away = glm::normalize(away);
        m_position += away * kPushStrength * 0.1f;
    }
}

void Bird::update(
    float dt,
    GLFWwindow* window,
    float terrainHeight,
    glm::vec3 terrainNormal
)
{
    g_DebugBirdPosition = m_position;
    g_DebugBirdVelocity = m_velocity;
    g_DebugBirdSpeed    = m_speed;
    g_DebugBirdRotation = m_rotation;

    // =====================================================
    // MODO ANDANDO
    // =====================================================

    if (standing)
    {
        float yawInput = 0.0f;

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            yawInput += 1.0f;

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            yawInput -= 1.0f;

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            yawInput *= 0.5f;

        float forwardInput = 0.0f;

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            forwardInput += 1.0f;

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            forwardInput -= 1.0f;

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            forwardInput *= 0.5f;

        m_yaw += yawInput * kYawSpeed * dt;

        glm::vec3 forward = getForward();

        float speed = forwardInput * WALK_SPEED;

        m_position += forward * speed * dt;

        // -----------------------------------------
        // Bob animation
        // -----------------------------------------

        m_walkTime += dt;

        float bobVertical =
            BOB_AMPLITUDE *
            sin(BOB_FREQUENCY * m_walkTime * 2.0f * M_PI);

        float bobRoll =
            BOB_AMPLITUDE_LATERAL *
            sin(BOB_FREQUENCY * m_walkTime * 2.0f * M_PI);

        m_baseY =
            terrainHeight + 0.5f;

        m_position.y =
            m_baseY + bobVertical;

        m_pitch = 0.0f;
        m_roll  = bobRoll;

        // -----------------------------------------
        // Decolagem
        // -----------------------------------------

        bool flapPressed =
            glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;

        if (flapPressed && !m_flapHeld)
        {
            standing = false;

            m_velocity = glm::vec3(0.0f);

            m_velocity.y =
                FLAP_IMPULSE_VERTICAL * 1.2f;

            m_velocity +=
                forward *
                (FLAP_IMPULSE_FORWARD * 1.5f);
        }

        m_flapHeld = flapPressed;

        // -----------------------------------------
        // Rotação do modelo em solo
        // -----------------------------------------

        const float MODEL_OFFSET =
            -M_PI / 2.0f;

        m_rotation =
            glm::vec3(
                0.0f,
                m_yaw + MODEL_OFFSET,
                0.0f
            );

        setRotation(m_rotation);

        updateDrawable();
        updateColliders();

        return;
    }

    // =====================================================
    // MODO VOANDO
    // =====================================================

    bool flapPressedFlying =
        glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;

    if (flapPressedFlying && !m_flapHeld)
    {
        m_velocity.y += FLAP_IMPULSE_VERTICAL;
        m_speed += 2.0f;
    }

    m_flapHeld = flapPressedFlying;

    // -----------------------------------------
    // Aceleração
    // -----------------------------------------

    float speedInput = 0.0f;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        speedInput += 1.0f;

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        speedInput -= 1.0f;

    m_speed += speedInput * ACCELERATION * dt;

    m_speed =
        glm::clamp(
            m_speed,
            SPEED_MIN,
            SPEED_MAX
        );

    // -----------------------------------------
    // Yaw
    // -----------------------------------------

    float yawInput = 0.0f;

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        yawInput += 1.0f;

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        yawInput -= 1.0f;

    m_yaw += yawInput * kYawSpeed * dt;

    // -----------------------------------------
    // Pitch automático
    // -----------------------------------------

    float verticalSpeed =
        m_velocity.y;

    float horizontalSpeed =
        glm::length(
            glm::vec2(
                m_velocity.x,
                m_velocity.z
            )
        );

    if (horizontalSpeed > 0.1f)
    {
        float targetPitch =
            atan2(
                verticalSpeed,
                horizontalSpeed
            );

        m_pitch =
            glm::mix(
                m_pitch,
                targetPitch,
                0.05f
            );
    }
    else
    {
        m_pitch =
            glm::mix(
                m_pitch,
                0.0f,
                0.05f
            );
    }

    m_pitch =
        glm::clamp(
            m_pitch,
            -kPitchMax,
            kPitchMax
        );

    // -----------------------------------------
    // Roll
    // -----------------------------------------

    float rollTarget =
        -yawInput * kRollTarget;

    m_roll +=
        (rollTarget - m_roll) *
        kRollSmooth *
        dt;

    // -----------------------------------------
    // Física
    // -----------------------------------------

    glm::vec3 forward =
        getForward();

    glm::vec3 desired =
        forward * m_speed;

    m_velocity.x =
        glm::mix(
            m_velocity.x,
            desired.x,
            kVelBlendXZ * dt
        );

    m_velocity.z =
        glm::mix(
            m_velocity.z,
            desired.z,
            kVelBlendXZ * dt
        );

    m_velocity.y =
        glm::mix(
            m_velocity.y,
            desired.y,
            kVelBlendY * dt
        );

    m_velocity.y +=
        m_speed *
        m_speed *
        kLiftCoeff *
        dt;

    m_velocity.y -=
        kGravity *
        dt;

    m_position +=
        m_velocity * dt;

    clampPosition();

    m_baseY =
        m_position.y;

    m_rotation =
        glm::vec3(
            m_pitch,
            m_yaw,
            m_roll
        );

    setRotation(m_rotation);

    updateDrawable();
    updateColliders();
}

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
}

/**
 * @brief   Limites de altura para o pássaro
 * 
 */
void Bird::clampPosition() {
    //  Limite inferior
    if(m_position.y < kHeightMin) { m_position.y = kHeightMin; }

    //  Limite superior
    if(m_position.y > kHeightMax) { m_position.y = kHeightMax; }
}