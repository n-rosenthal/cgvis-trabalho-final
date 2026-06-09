#pragma once

#ifndef BIRD_HPP
#define BIRD_HPP

#include <memory>
#include <vector>

#include "Objects/Assets.hpp"

#include <GLFW/glfw3.h>

#include "Objects/Interfaces/GameObject.hpp"
#include "Objects/Interfaces/Collidable.hpp"
#include "Objects/ObjDrawable.hpp"
#include "Collision/CapsuleCollider.hpp"
#include "Objects/Letter.hpp"

class Bird : public GameObject,
             public Collidable
{
public:
    Bird();

    void update(float dt, GLFWwindow* window);

    void updateColliders() override;
    void onCollision(glm::vec3 obstaclePos) override;
    bool onTerrainCollision(
        float terrainHeight,
        glm::vec3 terrainNormal
    );

    glm::vec3 getForward() const;
    glm::vec3 getUp() const;

    const CapsuleCollider& getCollider() const;

    std::vector<
        std::shared_ptr<Collider>
    > getColliders() override;

    bool getStanding() const
    {
        return standing;
    }

    glm::vec3 getSize() const
    {
        return size;
    }

    void setSize(const glm::vec3& s)
    {
        size = s;
    }

    static constexpr float DEFAULT_SIZE = 1.2f;
    static constexpr float STANDING_SCALE_FACTOR = 0.3f;

    void updateDrawable();

    bool carryingLetter() const;

    void pickLetter(
        std::shared_ptr<Letter> letter
    );

    void dropLetter();

    std::shared_ptr<Letter>
    getCarriedLetter() const
    {
        return m_carriedLetter;
    }

private:

    glm::mat4 rotationMatrix() const;
    void clampPosition();

    float m_yaw   = 0.0f;
    float m_pitch = 0.0f;
    float m_roll  = 0.0f;

    glm::vec3 m_velocity =
        glm::vec3(0.0f);

    float m_speed = 18.0f;

    CapsuleCollider m_collider;

    float m_hitCooldown     = 0.0f;
    float m_terrainCooldown = 0.0f;

    bool m_flapHeld = false;

    const ModelDefinition*
        m_currentModel;

    std::shared_ptr<Letter>
        m_carriedLetter;

    glm::vec3 size =
        glm::vec3(DEFAULT_SIZE);

    bool standing = false;
};

#endif