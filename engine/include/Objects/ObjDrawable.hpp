#pragma once

/**
 * @file    `ObjDrawable.hpp`
 * @brief   Header para implementação de `ObjDrawable`, uma classe de alto-nível para representar um objeto do tipo OBJ (wavefront) na cena virtual.
 */

#include <string>
#include <vector>

#include "Objects/Assets.hpp"
#include "Objects/Interfaces/Drawable.hpp"
#include "Loaders/ObjLoader.hpp"
#include "Renderer/ShaderLoader.hpp"

class ObjDrawable : public Drawable {
public:
    explicit ObjDrawable(
        const ModelDefinition& model
    );

    void buildMesh() override {};
    void computeNormals() override {};
    void setupBuffers() override {};

protected:
    void draw(const DrawContext&) override;

private:
    const ModelDefinition* m_model;
};
