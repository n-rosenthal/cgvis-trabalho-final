/**
 * @file    `Assets.cpp`
 * @brief   Definição de modelos, texturas e caminhos para modelos OBJ
 */

#include "Objects/Assets.hpp"
#include "Loaders/TextureLoader.hpp"

inline std::string asset_path(const std::string& sub) {
    return std::string("assets/") + sub;
}

namespace Assets {
    // =====================================================
    // TEXTURAS
    // =====================================================
    TextureDefinition TREE_1_TRUNK;
    TextureDefinition TREE_1_TWIGS;
    TextureDefinition TREE_1_LEAVES;

    TextureDefinition TREE_2_TRUNK;
    TextureDefinition TREE_2_TWIGS;
    TextureDefinition TREE_2_LEAVES;

    TextureDefinition RED_BRICKS;
    TextureDefinition ROCKY_TERRAIN;

    TextureDefinition BIRD;
    TextureDefinition BIRD_STANDING;

    TextureDefinition BUTTERFLY_ALBEDO_TRANSPARENCY;
    TextureDefinition BUTTERFLY_METTALLIC_SMOOTHNENESS;
    TextureDefinition BUTTERFLY_NORMAL;

    // =====================================================
    // MODELOS
    // =====================================================

    ModelDefinition TREE_1;
    ModelDefinition TREE_2;

    ModelDefinition LETTER;

    ModelDefinition BIRD_MODEL;
    ModelDefinition BIRD_STANDING_MODEL;

    ModelDefinition HOUSE;

    ModelDefinition BUTTERFLY;
};

void Assets::LoadTextures() {
    //  Árvores

    // =====================================================
    // TREE 1
    //      TextureDefinition TREE_1_TRUNK;
    //      TextureDefinition TREE_1_TWIGS;
    //      TextureDefinition TREE_1_LEAVES;
    // =====================================================

    TREE_1_TRUNK.file =
        asset_path(
            "textures/tree1_textures/"
            "Tree_1_Trunk_Limbs-DIFFUSE.png"
        );

    TREE_1_TWIGS.file =
        asset_path(
            "textures/tree1_textures/"
            "Tree_1_Twigs-DIFFUSE.jpg"
        );

    TREE_1_LEAVES.file =
        asset_path(
            "textures/tree1_textures/"
            "Tree_1_Leaf.png"
        );

    TREE_1_TRUNK.id =
        LoadTextureImage(
            TREE_1_TRUNK.file.c_str()
        );

    TREE_1_TWIGS.id =
        LoadTextureImage(
            TREE_1_TWIGS.file.c_str()
        );

    TREE_1_LEAVES.id =
        LoadTextureImage(
            TREE_1_LEAVES.file.c_str()
        );

    // =====================================================
    // BRICKS
    // =====================================================

    RED_BRICKS.file =
        asset_path(
            "textures/red_brick_diff_1k.jpg"
        );

    RED_BRICKS.id =
        LoadTextureImage(
            RED_BRICKS.file.c_str()
        );

    // =====================================================
    // TERRAIN
    // =====================================================

    ROCKY_TERRAIN.file =
        asset_path(
            "textures/rocky_terrain_02_diff_1k.jpg"
        );

    ROCKY_TERRAIN.id =
        LoadTextureImage(
            ROCKY_TERRAIN.file.c_str()
        );

    // =====================================================
    // BIRD
    // =====================================================

    BIRD.file =
        asset_path(
            "models/bird/falcon2.jpg"
        );

    BIRD.id =
        LoadTextureImage(
            BIRD.file.c_str()
        );

    BIRD_STANDING.file =
        asset_path(
            "models/bird/default-grey.jpg"
        );

    BIRD_STANDING.id =
        LoadTextureImage(
            BIRD_STANDING.file.c_str()
        );

    // =====================================================
    // BUTTERFLY
    // =====================================================

    BUTTERFLY_ALBEDO_TRANSPARENCY.file =
        asset_path(
            "textures/butterfly/butt_low_lambert1_AlbedoTransparency.png"
        );

    BUTTERFLY_ALBEDO_TRANSPARENCY.id =
        LoadTextureImage(
            BUTTERFLY_ALBEDO_TRANSPARENCY.file.c_str()
        );

    BUTTERFLY_METTALLIC_SMOOTHNENESS.file =
        asset_path(
            "textures/butterfly/butt_low_lambert1_MetallicSmoothness.png"
        );

    BUTTERFLY_METTALLIC_SMOOTHNENESS.id =
        LoadTextureImage(
            BUTTERFLY_METTALLIC_SMOOTHNENESS.file.c_str()
        );
    
    BUTTERFLY_NORMAL.file =
        asset_path(
            "textures/butterfly/butt_low_lambert1_Normal.png"
        );

    BUTTERFLY_NORMAL.id =
        LoadTextureImage(
            BUTTERFLY_NORMAL.file.c_str()
        );
};

void Assets::BuildModels() {
    // =====================================================
    //  TREE 1
    //      objFile: models/trees/Birch_Summer_1.obj
    //      meshes:
    // =====================================================
    TREE_1.objFile =
            asset_path(
                "models/trees/GenTree-103_AE3D_03122023-F1.obj"
            );

    TREE_1.meshes =
    {
        "leaves_Material-Leaves",
        "GenTree-Main_Trunk_Material.Trunk_and_Primary_Limbs",
        "GenTree_-_Twigs_Leaf_Bearing_Material.Twigs"
    };

    TREE_1.textures =
    {
        &TREE_1_LEAVES,
        &TREE_1_TRUNK,
        &TREE_1_TWIGS
    };

    TREE_1.alphaCutout = true;

    // =====================================================
    // BIRD
    // =====================================================

    BIRD_MODEL.objFile =
        asset_path(
            "models/bird/0V3HJRW3DQ5QPF3J2O5PR4Z1M.obj"
        );

    BIRD_MODEL.meshes =
    {
        "the_bird"
    };

    BIRD_MODEL.textures =
    {
        &BIRD
    };

    // =====================================================
    // BIRD STANDING
    // =====================================================

    BIRD_STANDING_MODEL.objFile =
        asset_path(
            "models/bird_standing/G1FXKUZIFSQHX0QERXO6AAO63.obj"
        );

    BIRD_STANDING_MODEL.meshes =
    {
        "the_bird2",
        "Object_color_0.031360-0.009440-0.009440.jpg"
    };

    BIRD_STANDING_MODEL.textures =
    {
        &BIRD_STANDING,
        nullptr
    };

    // =====================================================
    // LETTER
    // =====================================================

    LETTER.objFile =
        asset_path(
            "models/the_letter.obj"
        );

    LETTER.meshes =
    {
        "the_letter"
    };

    LETTER.textures = {};

    for (auto const& mesh : BIRD_STANDING_MODEL.meshes)
    {
        printf("standing mesh: %s\n", mesh.c_str());
    }

    // =====================================================
    //  Butterfly
    // =====================================================

    BUTTERFLY.objFile =
        asset_path(
            "models/butterfly/butterfly.obj"
        );
    
    BUTTERFLY.meshes =
    {
        "Butterfly_tri_torso_leg_01",
        "wing_01",
        "wing_02"
    };
    
    BUTTERFLY.textures =
    {
        &BUTTERFLY_ALBEDO_TRANSPARENCY,
        &BUTTERFLY_METTALLIC_SMOOTHNENESS,
        &BUTTERFLY_NORMAL
    };
    
    
};

void Assets::LoadAll() {
    LoadTextures();
    BuildModels();
}