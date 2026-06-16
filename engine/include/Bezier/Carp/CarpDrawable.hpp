#pragma once

#include "Objects/Interfaces/Drawable.hpp"
#include "GLFW/glfw3.h"

class CarpDrawable : public Drawable {
public:
    CarpDrawable() = default;

    void buildMesh() override {}
    void computeNormals() override {}
    void setupBuffers() override {}

    void model(
        const DrawContext& ctx
    ) override;

protected:
    void draw(
        const DrawContext& ctx
    ) override;
};