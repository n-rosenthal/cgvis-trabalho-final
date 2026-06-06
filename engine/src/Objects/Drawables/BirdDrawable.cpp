
/**
 * @file    BirdDrawable.cpp
 */
#include "Game/BirdDrawable.hpp"
#include "Loaders/ObjLoader.hpp"   // DrawVirtualObject

void BirdDrawable::draw(const DrawContext& ctx) {
    // Nome do mesh conforme registrado pelo ObjLoader ao carregar o .obj
    DrawVirtualObject("the_bird");
}
