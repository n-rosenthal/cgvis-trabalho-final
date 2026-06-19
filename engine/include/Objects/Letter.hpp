/**
 * @file    Letter.hpp
 * @brief   Header for the Letter class (floating collectible)
 */

#pragma once

#include "Objects/Interfaces/GameObject.hpp"
#include "Objects/Assets.hpp"
#include "Objects/ObjDrawable.hpp"
#include <memory>

class Letter : public GameObject {
public:
    /**
     * @brief Constructor
     * @param position  initial world position
     * @param rotation  initial rotation
     * @param scale     initial scale
     */
    Letter(
        const glm::vec3& position = glm::vec3(5.0f, -0.9f, 5.0f),
        const glm::vec3& rotation = glm::vec3(0.0f),
        const glm::vec3& scale    = glm::vec3(1.0f)
    ) : GameObject(std::make_unique<ObjDrawable>(Assets::LETTER), position, rotation, scale) {}

    // ---- Getters ----
    glm::vec3 getPosition() const { return m_position; }
    float getSize() const { return m_size; }

    // Ground position (the (x,z) where the letter rests)
    glm::vec2 getGroundPos() const { return m_groundPos; }
    float getFloatHeight() const { return m_floatHeight; }
    float getAmplitude() const { return m_amplitude; }
    float getPhase() const { return m_phase; }

    // ---- Setters ----
    void setPosition(const glm::vec3& pos) { m_position = pos; }
    void setRotation(const glm::vec3& rot) { m_rotation = rot; }
    void setSize(float size) { m_size = size; }

    void setGroundPos(const glm::vec2& pos) { m_groundPos = pos; }
    void setFloatHeight(float h) { m_floatHeight = h; }
    void setAmplitude(float a) { m_amplitude = a; }
    void setPhase(float p) { m_phase = p; }

private:
    float m_size = 1.0f;                     // scale factor (if needed separately)

    // Floating parameters (used when OnGround)
    glm::vec2 m_groundPos = glm::vec2(0.0f); // (x,z) where the letter sits
    float m_floatHeight = 2.0f;              // base height above terrain (h)
    float m_amplitude   = 0.5f;              // oscillation amplitude (d)
    float m_phase       = 0.0f;              // initial phase offset for oscillation

    std::unique_ptr<ObjDrawable> m_drawable;
};

enum class LetterState {
    OnGround,
    Carried,
    Falling
};