
/**
 * @file    BirdDrawable.cpp
 */
#include "Game/BirdDrawable.hpp"
#include "Loaders/ObjLoader.hpp"   // DrawVirtualObject

void BirdDrawable::draw(const DrawContext& ctx) {
    DrawVirtualObject("the_bird");
}
