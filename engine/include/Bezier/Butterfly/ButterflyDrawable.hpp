#pragma once

#include "Objects/Interfaces/Drawable.hpp"
#include "GLFW/glfw3.h" //  -> glfwGetTime() para animação

class ButterflyDrawable : public Drawable
{
public:
    ButterflyDrawable() = default;

    void buildMesh() override {}
    void computeNormals() override {}
    void setupBuffers() override {}

    void update(float dt);
    void model(const DrawContext& ctx) override;

protected:
    void draw(const DrawContext& ctx) override;

private:
    float m_animationTime = 0.0f;
};