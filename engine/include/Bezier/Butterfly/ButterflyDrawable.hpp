#pragma once

#include "Objects/Interfaces/Drawable.hpp"

class ButterflyDrawable : public Drawable
{
public:
    void draw(
        const DrawContext& ctx
    ) override;
};