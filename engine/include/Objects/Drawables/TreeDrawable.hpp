#pragma once

/**
 * @file    `TreeDrawable.hpp`
 * @brief   Header para impl. da interface `Drawable` (objecto renderizável da cena virtual)
 *          para uma árvore
 */

#include "Objects/Interfaces/Drawable.hpp"

class TreeDrawable : public Drawable {
public:
    TreeDrawable(int type);
    void buildMesh() override {}
    void computeNormals() override {}
    void setupBuffers() override {}
    void draw(const DrawContext& ctx) override;
private:
    int m_type;
};