#include "Game/Bird.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace
{
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

    constexpr float kFlapImpulse = 9.0f;

    constexpr float kVelBlendXZ = 5.0f;
    constexpr float kVelBlendY  = 2.5f;

    constexpr float kSpeedMin = 5.0f;
    constexpr float kSpeedMax = 42.0f;

    constexpr float kHeightMin = -1.0f;
    constexpr float kHeightMax = 120.0f;
}

Bird::Bird() :
    //  Pássaro enquanto objeto do jogo
    GameObject(
        //  Desenhável do pássaro
        std::make_unique<ObjDrawable>(
            Assets::BIRD_MODEL
        ),
        //  Posição inicial
        glm::vec3(
            0.0f,
            5.0f,
            0.0f
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
        &Assets::BIRD_MODEL;
}

/**
 * @brief   Matriz de rotação do pássaro
 * @return glm::mat4 
 */
glm::mat4 Bird::rotationMatrix() const {
    glm::mat4 r(1.0f);

    r = glm::rotate(
        r,
        m_yaw,
        glm::vec3(0,1,0)
    );

    r = glm::rotate(
        r,
        m_pitch,
        glm::vec3(1,0,0)
    );

    r = glm::rotate(
        r,
        m_roll,
        glm::vec3(0,0,1)
    );

    return r;
}

/**
 * @brief   Direção para frente
 * 
 * @return glm::vec3 
 */
glm::vec3 Bird::getForward() const {
    return glm::normalize(
        glm::vec3(
            rotationMatrix()
            *
            glm::vec4(
                -1,
                0,
                0,
                0
            )
        )
    );
}

/**
 * @brief   Direção para cima
 * 
 * @return glm::vec3 
 */
glm::vec3 Bird::getUp() const
{
    return glm::normalize(
        glm::vec3(
            rotationMatrix()
            *
            glm::vec4(
                0,
                1,
                0,
                0
            )
        )
    );
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
    //  Quando colide com o chão, o pássaro fica parado
    standing = true;

    //  Zera a velocidade
    m_velocity = glm::vec3(0.0f);

    //  Posiciona o pássaro no chão
    m_position.y =
        terrainHeight + 0.6f;

    return true;
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
void Bird::update(float dt,  GLFWwindow* window) {
    //  < ESPAÇO >: "bater asas" = aumentar velocidade
    bool flapPressed = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
    if(flapPressed && !m_flapHeld) {
        //  Se estiver parado, começa a voar
        standing = false;

        //  Aumenta a velocidade por um impulso
        m_velocity.y += kFlapImpulse;
    };

    //  Atualiza o estado das asas
    m_flapHeld = flapPressed;

    //  < A > e < D >: Controle angular
    float yawInput = 
            (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS ? 1.0f : 0.0f)
        +   (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS ? -1.0f : 0.0f);

    //  < W > e < S >: Controle vertical
    float pitchInput =
            (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS ? 1.0f : 0.0f)
        +   (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS ? -1.0f : 0.0f);

    //  Controle angular (eixo Y)
    m_yaw   +=  yawInput
            *   kYawSpeed
            *   dt;

    //  Controle vertical (eixo X)
    m_pitch +=  pitchInput
            *   kPitchSpeed
            *   dt;

    //  Limita o pitch para evitar o flip
    m_pitch = glm::clamp(m_pitch, -kPitchMax, kPitchMax);

    //  Controle de roll (eixo Z)
    float rollTarget = -yawInput
                     *  kRollTarget;

    //  Suaviza o roll
    m_roll          += (rollTarget - m_roll)
                    *   kRollSmooth
                    *   dt;

    //  Atualiza a velocidade
    glm::vec3 forward   =   getForward();
    glm::vec3 desired   =   forward * m_speed;

    m_velocity.x        =   glm::mix(m_velocity.x, desired.x, kVelBlendXZ * dt);
    m_velocity.z        =   glm::mix(m_velocity.z, desired.z, kVelBlendXZ * dt);
    m_velocity.y        =   glm::mix(m_velocity.y, desired.y, kVelBlendY * dt);
    m_velocity.y        +=  m_speed
                         *  m_speed
                         *  kLiftCoeff
                         *  dt;
    m_velocity.y        -=  kGravity
                         *  dt;

    //  Atualiza a posição
    m_position  +=  m_velocity
                 *  dt;

    //  Limita a altura
    clampPosition();

    //  Atualiza o modelo
    updateDrawable();

    //  Atualiza a rotação
    m_rotation  = glm::vec3(m_pitch, m_yaw + glm::pi<float>(), m_roll);

    //  Atualiza os colisores
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
