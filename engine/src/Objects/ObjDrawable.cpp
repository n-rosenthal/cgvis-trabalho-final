#include "Objects/ObjDrawable.hpp"
#include "Renderer/Textures.hpp"
#include <glad/glad.h>

ObjDrawable::ObjDrawable(
    std::vector<std::string> objects,
    TextureSet texture
)
:
    m_objects(std::move(objects)),
    m_texture(texture)
{
}

static void bindTextureSet(TextureSet texture)
{
    switch(texture)
    {
        case TextureSet::TREE:
            glBindTexture(GL_TEXTURE_2D, Textures::TREE);
            break;

        case TextureSet::HOUSE:
            glBindTexture(GL_TEXTURE_2D, Textures::HOUSE);
            break;

        case TextureSet::ROCK:
            glBindTexture(GL_TEXTURE_2D, Textures::ROCK);
            break;

        case TextureSet::LETTER:
            glBindTexture(GL_TEXTURE_2D, Textures::LETTER);
            break;

        case TextureSet::BIRD:
            glBindTexture(GL_TEXTURE_2D, Textures::BIRD);
            break;

        default:
            break;
    }
}

void ObjDrawable::draw(const DrawContext&)
{
    bindTextureSet(m_texture);

    for(const auto& object : m_objects)
    {
        DrawVirtualObject(object.c_str());
    }
}