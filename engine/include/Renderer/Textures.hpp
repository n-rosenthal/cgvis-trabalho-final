#pragma once

#include <glad/glad.h>

namespace Textures
{
    extern GLuint TREE;
    extern GLuint HOUSE;
    extern GLuint ROCK;
    extern GLuint LETTER;
    extern GLuint BIRD;

    void LoadAll();
    void DestroyAll();
}