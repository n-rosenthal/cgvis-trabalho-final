#pragma once

#include "Objects/Interfaces/Drawable.hpp"

class HouseDrawable : public Drawable
{
public:
    HouseDrawable();

    void buildMesh() override;
    void computeNormals() override;
    void setupBuffers() override;

    void setPosition(const glm::vec3& position);

protected:
    void draw(const DrawContext& ctx) override;

private:
    glm::mat4 m_modelMatrix; 
};