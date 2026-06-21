#pragma once

#include "Objects/Interfaces/GameObject.hpp"
#include "Objects/Interfaces/Collidable.hpp"

#include "Objects/Assets.hpp"
#include "Objects/ObjDrawable.hpp"

#include "Collision/SphereCollider.hpp"

#include <memory>
#include <vector>

class Letter : public GameObject, public Collidable {
public:
    Letter(
        const glm::vec3& position = glm::vec3(5.0f, -0.9f, 5.0f),
        const glm::vec3& rotation = glm::vec3(0.0f),
        const glm::vec3& scale    = glm::vec3(1.0f)
    );

    // ---------------------------
    // Collidable
    // ---------------------------
    std::vector<std::shared_ptr<Collider>> getColliders() const override;
    void updateColliders() override;
    void onCollision(glm::vec3 objectPosition) override;

    // ---------------------------
    // Getters
    // ---------------------------
    glm::vec3 getPosition() const { return m_position; }
    float getSize() const { return m_size; }

    glm::vec2 getGroundPos() const { return m_groundPos; }
    float getFloatHeight() const { return m_floatHeight; }
    float getAmplitude() const { return m_amplitude; }
    float getPhase() const { return m_phase; }

    // ---------------------------
    // Setters
    // ---------------------------
    void setPosition(const glm::vec3& pos);
    void setRotation(const glm::vec3& rot);
    void setSize(float size);

    void setGroundPos(const glm::vec2& pos) { m_groundPos = pos; }
    void setFloatHeight(float h) { m_floatHeight = h; }
    void setAmplitude(float a) { m_amplitude = a; }
    void setPhase(float p) { m_phase = p; }

private:
    float m_size = 1.0f;

    glm::vec2 m_groundPos = glm::vec2(0.0f);
    float m_floatHeight = 2.0f;
    float m_amplitude   = 0.5f;
    float m_phase       = 0.0f;

    std::shared_ptr<SphereCollider> m_collider;
};

enum class LetterState {
    OnGround,
    Carried,
    Falling,
    Thrown
};