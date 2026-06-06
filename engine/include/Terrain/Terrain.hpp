#pragma once

#include "Objects/Interfaces/Drawable.hpp"

class Terrain : public Drawable
{
public:
    Terrain(
        int width,
        int depth,
        float spacing
    );

    float getHeight(float x, float z) const;
    glm::vec3 getNormal(float x, float z) const;

protected:
    void buildMesh() override;
    void computeNormals() override;
    void setupBuffers() override;
    void draw(const DrawContext& ctx) override;

private:
    int   m_width;
    int   m_depth;
    float m_spacing;

    float sampleHeight(float x,float z) const;
    glm::vec3 sampleColor(float h) const;
};