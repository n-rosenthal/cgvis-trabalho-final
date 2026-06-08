#pragma once

/**
 * @file    `ObjDrawable.hpp`
 * @brief   Header para implementação de `ObjDrawable`, uma classe de alto-nível para representar um objeto do tipo OBJ (wavefront) na cena virtual.
 */

#include <string>
#include <vector>

#include "Objects/Interfaces/Drawable.hpp"
#include "Loaders/ObjLoader.hpp"

enum class TextureSet
{
    NONE,

    TREE,
    HOUSE,
    ROCK,
    BRICK,
    METAL,

    BIRD,
    LETTER
};

class ObjDrawable : public Drawable
{
public:
    ObjDrawable(
        std::vector<std::string> objects,
        TextureSet texture = TextureSet::NONE
    );

    void buildMesh() override {}
    void computeNormals() override {}
    void setupBuffers() override {}

protected:
    void draw(const DrawContext&) override;

private:
    std::vector<std::string> m_objects;
    TextureSet m_texture;
};