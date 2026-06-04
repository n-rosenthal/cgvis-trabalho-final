/**
 * @file    `TreeDrawable.cpp`
 * @brief   Implementação de uma interface de renderização para uma árvore
 */
#include "Objects/Drawables/TreeDrawable.hpp"
#include "Loaders/ObjLoader.hpp"   // para DrawVirtualObject

TreeDrawable::TreeDrawable(int type) : m_type(type) {}

void TreeDrawable::draw(const DrawContext& ctx) {
    if (m_type == 1) {  // tree2
        DrawVirtualObject("GenTree_100-Main_Trunk_Material.012_-_TRUNK");
        DrawVirtualObject("leaves.001_Material.010_-_Leaves");
        DrawVirtualObject("GenTRee_100-Branches_L2.002_Material.011_-_Secondary_Limbs");
        DrawVirtualObject("GenTRee_100-Branches_L2.002_Material");
    } else {            // tree1 (incluindo m_type == 0)
        DrawVirtualObject("GenTree_-_Twigs_Leaf_Bearing_Material.Twigs");
        DrawVirtualObject("leaves_Material-Leaves");
        DrawVirtualObject("GenTree-Main_Trunk_Material.Trunk_and_Primary_Limbs");
    }
}