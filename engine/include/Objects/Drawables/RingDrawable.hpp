/**
 * @file    RingDrawable.hpp
 * @brief   Drawable para o anel de objetivo.
 *          Gera a geometria de anel (torus plano) e
 *          aplica billboard + pulsação no draw.
 */
#pragma once

#include "Objects/Interfaces/Drawable.hpp"

class RingDrawable : public Drawable {
public:
    explicit RingDrawable(float radius);

    void buildMesh()      override;
    void computeNormals() override {}   // normais fixas, geradas em buildMesh
    void setupBuffers()   override;

    // Atualiza estado de animação — chamado por Ring::update
    void setPulseTime(float t)   { m_pulseTime = t; }
    void setAnimScale(float s)   { m_animScale = s; }
    void setViewMatrix(const glm::mat4& v) { m_view = v; }

protected:
    void draw(const DrawContext& ctx) override;

private:
    float     m_radius;
    float     m_pulseTime = 0.0f;
    float     m_animScale = 1.0f;
    glm::mat4 m_view      = glm::mat4(1.0f);

    int m_indexCount = 0;
};