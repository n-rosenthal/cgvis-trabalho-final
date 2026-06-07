#pragma once

#include "Objects/Interfaces/Drawable.hpp"

class HouseDrawable : public Drawable
{
public:
    HouseDrawable();

    void buildMesh() override;
    void computeNormals() override;
    void setupBuffers() override;

protected:
    void draw(const DrawContext& ctx) override;
};