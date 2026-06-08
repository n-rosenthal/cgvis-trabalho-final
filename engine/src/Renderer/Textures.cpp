#include "Renderer/Textures.hpp"
#include "Loaders/TextureLoader.hpp"

namespace Textures
{
    GLuint TREE   = 0;
    GLuint HOUSE  = 0;
    GLuint ROCK   = 0;
    GLuint LETTER = 0;
    GLuint BIRD   = 0;
}

void Textures::LoadAll()
{
    TREE =
        LoadTextureImage(
            "assets/textures/tree1_textures/Leaf_Oak_Gum_AE3D_10302022-A.png"
        );

    HOUSE =
        LoadTextureImage(
            "assets/textures/red_brick_diff_1k.jpg"
        );

    ROCK =
        LoadTextureImage(
            "assets/textures/rocky_terrain_02_diff_1k.jpg"
        );

    // LETTER =
    //     LoadTextureImage(
    //         "assets/textures/paper.png"
    //     );

    BIRD =
        LoadTextureImage(
            "assets/models/bird/falcon2.jpg"
        );
}

void Textures::DestroyAll()
{
    glDeleteTextures(1, &TREE);
    glDeleteTextures(1, &HOUSE);
    glDeleteTextures(1, &ROCK);
    glDeleteTextures(1, &LETTER);
    glDeleteTextures(1, &BIRD);
}