#include "Bezier/Butterfly/ButterflyDrawable.hpp"
#include <cstdio>

extern void DrawVirtualObject(const char* name);

void ButterflyDrawable::draw(
    const DrawContext& ctx
)
{
    printf("DRAWING BUTTERFLY\n");
 
    DrawVirtualObject(
        "Butterfly_tri_torso_leg_01"
    );

    DrawVirtualObject(
        "wing_01"
    );

    DrawVirtualObject(
        "wing_02"
    );
}