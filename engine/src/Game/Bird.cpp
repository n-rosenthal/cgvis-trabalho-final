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

Bird::Bird()
:
GameObject(
    std::make_unique<ObjDrawable>(
        Assets::BIRD_MODEL
    ),
    glm::vec3(
        0.0f,
        5.0f,
        0.0f
    ),
    glm::vec3(0.0f),
    glm::vec3(DEFAULT_SIZE)
),
m_collider(
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
    1.0f
)
{
    m_currentModel =
        &Assets::BIRD_MODEL;
}

glm::mat4 Bird::rotationMatrix() const
{
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

glm::vec3 Bird::getForward() const
{
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

const CapsuleCollider&
Bird::getCollider() const
{
    return m_collider;
}

std::vector<
    std::shared_ptr<Collider>
>
Bird::getColliders()
{
    return {
        std::make_shared<
            CapsuleCollider
        >(m_collider)
    };
}

void Bird::updateDrawable()
{
    const ModelDefinition*
        desired =
            standing
            ? &Assets::BIRD_STANDING_MODEL
            : &Assets::BIRD_MODEL;

    if(desired == m_currentModel)
        return;

    m_currentModel = desired;

    m_drawable =
        std::make_unique<
            ObjDrawable
        >(
            *desired
        );
}

bool Bird::onTerrainCollision(
    float terrainHeight,
    glm::vec3 terrainNormal
)
{
    standing = true;

    m_position.y =
        terrainHeight + 0.6f;

    return true;
}

void Bird::onCollision(
    glm::vec3 obstaclePos
)
{
}

void Bird::update(
    float dt,
    GLFWwindow* window
)
{
    bool flapPressed =
        glfwGetKey(
            window,
            GLFW_KEY_SPACE
        ) == GLFW_PRESS;

    if(
        flapPressed &&
        !m_flapHeld
    )
    {
        standing = false;

        m_velocity.y +=
            kFlapImpulse;
    }

    m_flapHeld =
        flapPressed;

    static bool gHeld = false;

    bool gPressed =
        glfwGetKey(
            window,
            GLFW_KEY_G
        ) == GLFW_PRESS;

    if(
        gPressed &&
        !gHeld
    )
    {
        dropLetter();
    }

    gHeld = gPressed;

    float yawInput =
        (glfwGetKey(
            window,
            GLFW_KEY_A
        ) == GLFW_PRESS ? 1.0f : 0.0f)
        +
        (glfwGetKey(
            window,
            GLFW_KEY_D
        ) == GLFW_PRESS ? -1.0f : 0.0f);

    float pitchInput =
        (glfwGetKey(
            window,
            GLFW_KEY_W
        ) == GLFW_PRESS ? 1.0f : 0.0f)
        +
        (glfwGetKey(
            window,
            GLFW_KEY_S
        ) == GLFW_PRESS ? -1.0f : 0.0f);

    m_yaw +=
        yawInput *
        kYawSpeed *
        dt;

    m_pitch +=
        pitchInput *
        kPitchSpeed *
        dt;

    m_pitch =
        glm::clamp(
            m_pitch,
            -kPitchMax,
            kPitchMax
        );

    float rollTarget =
        -yawInput *
        kRollTarget;

    m_roll +=
        (rollTarget - m_roll)
        *
        kRollSmooth
        *
        dt;

    glm::vec3 forward =
        getForward();

    glm::vec3 desired =
        forward *
        m_speed;

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
        m_velocity *
        dt;

    clampPosition();

    updateDrawable();

    m_rotation =
        glm::vec3(
            m_pitch,
            m_yaw +
            glm::pi<float>(),
            m_roll
        );

    updateColliders();

    if(m_carriedLetter)
    {
        glm::vec3 offset =
            getForward()*2.0f
            -
            getUp()*0.5f;

        m_carriedLetter
            ->setPosition(
                m_position +
                offset
            );

        m_carriedLetter
            ->setRotation(
                m_rotation
            );
    }
}

void Bird::updateColliders()
{
    glm::vec3 fwd =
        getForward();

    m_collider.p0 =
        m_position -
        fwd * 1.5f;

    m_collider.p1 =
        m_position +
        fwd * 1.5f;
}

void Bird::clampPosition()
{
    if(m_position.y < kHeightMin)
    {
        m_position.y =
            kHeightMin;
    }

    if(m_position.y > kHeightMax)
    {
        m_position.y =
            kHeightMax;
    }
}

bool Bird::carryingLetter() const
{
    return
        m_carriedLetter
        != nullptr;
}

void Bird::pickLetter(std::shared_ptr<Letter> letter)
{
    m_carriedLetter = letter;

    if(letter)
        letter->pickUp();
}

void Bird::dropLetter()
{
    if(!m_carriedLetter)
        return;

    glm::vec3 dropPos =
        m_position -
        getForward() * 2.0f;

    m_carriedLetter->setPosition(dropPos);

    m_carriedLetter->setCaptured(false);
    m_carriedLetter->drop();

    m_carriedLetter.reset();
}