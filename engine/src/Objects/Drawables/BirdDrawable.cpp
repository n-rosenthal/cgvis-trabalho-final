
/**
 * @file    BirdDrawable.cpp
 */
#include "Game/BirdDrawable.hpp"
#include "Loaders/ObjLoader.hpp"   // DrawVirtualObject
#include "Game/Renderer.hpp"

void BirdDrawable::draw(const DrawContext& ctx)
{
    if (ctx.object_id == Renderer::OBJ_BIRD2)
        DrawVirtualObject("the_bird2");
    else
        DrawVirtualObject("the_bird");
}