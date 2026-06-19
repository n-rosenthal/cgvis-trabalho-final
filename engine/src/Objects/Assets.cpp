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

    TextureDefinition BIRD;
    TextureDefinition BIRD_STANDING;
    TextureDefinition BIRD_STANDING_LEFT;

    TextureDefinition HOUSE_NORMAL;

    TextureDefinition MAILBOX_COLOR;

    //  Árvore BIRCH
    //  sem textura?

    //  Árvore PINE
    TextureDefinition PINE_TEXTURE;

    //  Árvore OAK
    TextureDefinition OAK_TEXTURE;
    TextureDefinition OAK_LEAVES_TEXTURE;

    //  Arbusto BUSH
    TextureDefinition BUSH_TEXTURE;

    //  Arbusto BUXUS
    TextureDefinition BUXUS_TEXTURE;

    //  Arbusto SHRUB
    TextureDefinition SHRUB_TEXTURE;

    //  Arbusto STYLED_SHRUB
    TextureDefinition STYLED_SHRUB_TEXTURE;

    //  Planta MARINE
    TextureDefinition MARINE_PLANT_TEXTURE;

    //  Flor MARGARIDA
    // TextureDefinition MARGARITE_TEXTURE;

    //  ROCHAS (1-6, HUGE_ROCK)
    TextureDefinition ROCK_1_TEXTURE;
    TextureDefinition ROCK_2_TEXTURE;
    TextureDefinition ROCK_3_TEXTURE;
    TextureDefinition ROCK_4_TEXTURE;
    TextureDefinition ROCK_5_TEXTURE;
    // TextureDefinition ROCK_6_TEXTURE;e
    // TextureDefinition HUGE_ROCK_TEXTURE;

    //  Pássaro voador (NPC)
    TextureDefinition FLYING_BIRD_TEXTURE;

    //  Pato (NPC)
    TextureDefinition DUCK_TEXTURE;

    //  NPCs
    TextureDefinition BUTTERFLY_ALBEDO_TRANSPARENCY;

    // =====================================================
    // MODELOS
    // =====================================================

    ModelDefinition TREE_1;
    ModelDefinition TREE_2;

    ModelDefinition LETTER;

    ModelDefinition BIRD_MODEL;
    ModelDefinition BIRD_STANDING_MODEL;

    ModelDefinition HOUSE;
    ModelDefinition MAILBOX;

    //  Árvore BIRCH
    ModelDefinition BIRCH;

    //  Árvore PINE
    ModelDefinition PINE;

    //  Árvore OAK
    ModelDefinition OAK;

    //  Arbusto BUSH
    ModelDefinition BUSH;

    //  Arbusto BUXUS
    ModelDefinition BUXUS;

    //  Arbusto SHRUB
    ModelDefinition SHRUB;

    //  Arbusto STYLED_SHRUB
    ModelDefinition STYLED_SHRUB;

    //  Planta MARINE
    ModelDefinition MARINE_PLANT;

    //  Flor MARGARIDA
    ModelDefinition MARGARITE;

    //  ROCHAS (1-6, HUGE_ROCK)
    ModelDefinition ROCK_1;
    ModelDefinition ROCK_2;
    ModelDefinition ROCK_3;
    ModelDefinition ROCK_4;
    ModelDefinition ROCK_5;
    // ModelDefinition ROCK_6;
    ModelDefinition HUGE_ROCK;

    //  Pássaro voador (NPC)
    ModelDefinition FLYING_BIRD;

    //  Pato (NPC)
    ModelDefinition DUCK;

    //  NPCs
    ModelDefinition BUTTERFLY;
    ModelDefinition CARP;
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
            TREE_1_TRUNK.file.c_str(),
            true
        );

    TREE_1_TWIGS.id =
        LoadTextureImage(
            TREE_1_TWIGS.file.c_str(),
            true
        );

    TREE_1_LEAVES.id =
        LoadTextureImage(
            TREE_1_LEAVES.file.c_str(),
            true
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
            BIRD.file.c_str(),
            true
        );

    BIRD_STANDING.file =
        asset_path(
            "models/bird_standing/default-grey.jpg"
        );

    BIRD_STANDING.id =
        LoadTextureImage(
            BIRD_STANDING.file.c_str(),
            true
        );
    
    BIRD_STANDING_LEFT.file =
        asset_path(
            "models/bird_standing/bird1l.jpg"
        );

    BIRD_STANDING_LEFT.id =
        LoadTextureImage(
            BIRD_STANDING_LEFT.file.c_str(),
            true
        );


    // =====================================================
    // HOUSE
    // =====================================================
    HOUSE_NORMAL.file =
        asset_path(
            "textures/house/Material__23_baseColor.jpeg"
        );

    HOUSE_NORMAL.id =
        LoadTextureImage(
            HOUSE_NORMAL.file.c_str(),
            true
        );

    // =====================================================
    // MAILBOX
    // =====================================================
    MAILBOX_COLOR.file =
        asset_path(
            "textures/mailbox/M_Mailbox_baseColor.png"
        );

    MAILBOX_COLOR.id =
        LoadTextureImage(
            MAILBOX_COLOR.file.c_str(),
            true
        );

    //  =====================================================
    //  Árvore BIRCH
    //  =====================================================
    //  sem textura?

    //  =====================================================
    //  Árvore PINE
    //  =====================================================
    PINE_TEXTURE.file =
        asset_path(
            "textures/pine_low_poly/Pine_s_1_Material_Base_Color.png"
        );

    PINE_TEXTURE.id =
        LoadTextureImage(
            PINE_TEXTURE.file.c_str(),
            true
        );

    //  =====================================================
    //  Árvore OAK
    //  =====================================================
    OAK_TEXTURE.file =
        asset_path(
            "textures/tree_oak/Cube_001_diffuse.png"
        );

    OAK_TEXTURE.id =
        LoadTextureImage(
            OAK_TEXTURE.file.c_str(),
            true
        );

    OAK_LEAVES_TEXTURE.file =
        asset_path(
            "textures/tree_oak/Plane_067_diffuse.png"
        );

    OAK_LEAVES_TEXTURE.id =
        LoadTextureImage(
            OAK_LEAVES_TEXTURE.file.c_str(),
            true
    );

    //  =====================================================
    //  Arbusto BUSH
    //  =====================================================
    BUSH_TEXTURE.file =
        asset_path(
            "textures/bush/vetka_kusta_0.png"
        );

    BUSH_TEXTURE.id =
        LoadTextureImage(
            BUSH_TEXTURE.file.c_str(),
            false
        );

    //  =====================================================
    //  Arbusto BUXUS
    //  =====================================================
    BUXUS_TEXTURE.file =
        asset_path(
            "textures/buxus/Buxus_Base_color.png"
        );

    BUXUS_TEXTURE.id =
        LoadTextureImage(
            BUXUS_TEXTURE.file.c_str(),
            true
        );

    //  =====================================================
    //  Arbusto SHRUB
    //  =====================================================
    SHRUB_TEXTURE.file =
        asset_path(
            "textures/low_poly_shrub/bush_PNG7214.png"
        );

    SHRUB_TEXTURE.id =
        LoadTextureImage(
            SHRUB_TEXTURE.file.c_str(),
            true
        );

    //  =====================================================
    //  Arbusto STYLED_SHRUB
    //  =====================================================
    STYLED_SHRUB_TEXTURE.file =
        asset_path(
            "textures/styled_bush/9.png"
        );

    STYLED_SHRUB_TEXTURE.id =
        LoadTextureImage(
            STYLED_SHRUB_TEXTURE.file.c_str(),
            true
        );
    
    //  =====================================================
    //  Planta MARINE
    //  =====================================================
    MARINE_PLANT_TEXTURE.file =
        asset_path(
            "textures/marine_plant/plant_1_diffuse.jpeg"
        );

    MARINE_PLANT_TEXTURE.id =
        LoadTextureImage(
            MARINE_PLANT_TEXTURE.file.c_str(),
            true
        );
    

    // =====================================================
    //  Flor MARGARIDA
    // =====================================================
    //  sem textura?

    // =====================================================
    //  ROCHAS (1-6, HUGE_ROCK)
    // =====================================================
    ROCK_1_TEXTURE.file =
        asset_path(
            "textures/rocks/rock1_lp_1001_BaseColor.png"
        );

    ROCK_1_TEXTURE.id =
        LoadTextureImage(
            ROCK_1_TEXTURE.file.c_str(),
            true
        );

    ROCK_2_TEXTURE.file =
        asset_path(
            "textures/rocks/rock3_lp_1001_BaseColor.png"
        );

    ROCK_2_TEXTURE.id =
        LoadTextureImage(
            ROCK_2_TEXTURE.file.c_str(),
            true
        );
    
    ROCK_3_TEXTURE.file =
        asset_path(
            "textures/rocks/rock4_lp_1001_BaseColor.png"
        );

    ROCK_3_TEXTURE.id =
        LoadTextureImage(
            ROCK_3_TEXTURE.file.c_str(),
            true
        );

    ROCK_4_TEXTURE.file =
        asset_path(
            "textures/rocks/rock5_lp_1001_BaseColor.png"
        );

    ROCK_4_TEXTURE.id =
        LoadTextureImage(
            ROCK_4_TEXTURE.file.c_str(),
            true
        );

    ROCK_5_TEXTURE.file =
        asset_path(
            "textures/rocks/rock6_lp_1001_BaseColor.png"
        );

    ROCK_5_TEXTURE.id =
        LoadTextureImage(
            ROCK_5_TEXTURE.file.c_str(),
            true
        );

    // =====================================================
    //  Pássaro voador (NPC)
    // =====================================================
    FLYING_BIRD_TEXTURE.file =
        asset_path(
            "textures/flying_bird/BirdUVTexture.jpeg"
        );
    
    FLYING_BIRD_TEXTURE.id =
        LoadTextureImage(
            FLYING_BIRD_TEXTURE.file.c_str(),
            true
        );


    // =====================================================
    //  Pato (NPC)
    // =====================================================
    DUCK_TEXTURE.file =
        asset_path(
            "textures/low_poly_duck/duck skin _0.png"
        );
    
    DUCK_TEXTURE.id =
        LoadTextureImage(
            DUCK_TEXTURE.file.c_str(),
            true
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
            BUTTERFLY_ALBEDO_TRANSPARENCY.file.c_str(),
            true
        );

    // =====================================================
    // CARP
    // =====================================================

    // CARP_NORMAL.file =
    //     asset_path(
    //         "textures/carp/gltf_embedded_0.jpeg"
    //     );
    
    // CARP_NORMAL.id =
    //     LoadTextureImage(
    //         CARP_NORMAL.file.c_str()
    //     );
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
        &BIRD_STANDING_LEFT,
        &BIRD_STANDING
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


    // =====================================================
    // HOUSE
    // =====================================================

    HOUSE.objFile =
        asset_path(
            "models/house/house.obj"
        );

    HOUSE.meshes =
    {
        "Object_3"
    };

    HOUSE.textures =
    {
        &HOUSE_NORMAL
    };

    HOUSE.useTexture = true;

    // =====================================================
    // MAILBOX
    // =====================================================

    MAILBOX.objFile =
        asset_path(
            "models/mailbox/mailbox.obj"
        );

    MAILBOX.meshes =
    {
        "Object_4"
    };

    MAILBOX.textures =
    {
        &MAILBOX_COLOR
    };

    MAILBOX.useTexture = true;

    //  =====================================================
    //  Árvore BIRCH
    //  =====================================================
    BIRCH.objFile =
        asset_path(
            "models/birch/birch.obj"
        );

    BIRCH.meshes =
    {
        "birchTrunk01",
        "birchTrunk02",
        "birchTrunk03",
        "birchTrunk04",
        "pineTreeColor05",
        "pineTreeColor06"
    };

    BIRCH.textures =
    {
        nullptr
    };

    BIRCH.useTexture = false;

    // =====================================================
    //  Árvore PINE
    //  =====================================================
    PINE.objFile =
        asset_path(
            "models/pine_low_poly/pine.obj"
        );

    PINE.meshes =
    {
        "Pine_s_1_Material"
    };

    PINE.textures =
    {
        &PINE_TEXTURE
    };

    PINE.useTexture = true;


    // =====================================================
    //  Árvore OAK
    //  =====================================================
    OAK.objFile =
        asset_path(
            "models/tree_oak/tree_oak.obj"
        );

    OAK.meshes =
    {
        "Cube_001",
        "Plane_067"
    };

    OAK.textures =
    {
        &OAK_TEXTURE,
        &OAK_LEAVES_TEXTURE
    };

    OAK.useTexture = true;
    // =====================================================
    //  Arbusto BUSH
    //  =====================================================
    BUSH.objFile =
        asset_path(
            "models/bush/bush.obj"
        );

    BUSH.meshes =
    {
        "vetka_kusta"
    };

    BUSH.textures =
    {
        &BUSH_TEXTURE
    };

    BUSH.useTexture = true;

    // =====================================================
    //  Arbusto BUXUS
    //  =====================================================
    BUXUS.objFile =
        asset_path(
            "models/buxus/buxus.obj"
        );

    BUXUS.meshes =
    {
        "standardSurface2"
    };

    BUXUS.textures =
    {
        &BUXUS_TEXTURE
    };

    BUXUS.useTexture = true;

    // =====================================================
    //  Arbusto SHRUB
    //  =====================================================
    SHRUB.objFile =
        asset_path(
            "models/shrub/low_poly_shrub.obj"
        );

    SHRUB.meshes =
    {
        "lambert2"
    };

    SHRUB.textures =
    {
        &SHRUB_TEXTURE
    };

    SHRUB.useTexture = true;

    // =====================================================
    //  Arbusto STYLED_SHRUB
    //  =====================================================
    STYLED_SHRUB.objFile =
        asset_path(
            "models/styled_shrub/styled_shrub.obj"
        );

    STYLED_SHRUB.meshes =
    {
        "stylized_bush"
    };

    STYLED_SHRUB.textures =
    {
        &STYLED_SHRUB_TEXTURE
    };

    STYLED_SHRUB.useTexture = true;

    // =====================================================
    //  Planta MARINE
    //  =====================================================
    MARINE_PLANT.objFile =
        asset_path(
            "models/marine_plant/marine_plant.obj"
        );

    MARINE_PLANT.meshes =
    {
        "blade"
    };

    MARINE_PLANT.textures =
    {
        &MARINE_PLANT_TEXTURE
    };

    MARINE_PLANT.useTexture = true;

    // =====================================================
    //  Flor MARGARIDA
    //  =====================================================
    MARGARITE.objFile =
        asset_path(
            "models/margarite/margarita_flower_bush.obj"
        );
    
    MARGARITE.meshes =
    {
        "green",
        "white",
        "yellow"
    };
    
    MARGARITE.textures =
    {
        nullptr
    };

    MARGARITE.useTexture = false;

    // =====================================================
    //  ROCHAS (1-5, HUGE_ROCK)
    //  =====================================================
    ROCK_1.objFile =
        asset_path(
            "models/rocks/rock1.obj"
        );

    ROCK_1.meshes =
    {
        "Rock_lp_1"
    };

    ROCK_1.textures =
    {
        &ROCK_1_TEXTURE
    };

    ROCK_1.useTexture = true;

    ROCK_2.objFile =
        asset_path(
            "models/rocks/rock3.obj"
        );

    ROCK_2.meshes =
    {
        "Rock_lp_3"
    };

    ROCK_2.textures =
    {
        &ROCK_2_TEXTURE
    };

    ROCK_2.useTexture = true;

    ROCK_3.objFile =
        asset_path(
            "models/rocks/rock4.obj"
        );

    ROCK_3.meshes =
    {
        "Rock_lp_4"
    };

    ROCK_3.textures =
    {
        &ROCK_3_TEXTURE
    };

    ROCK_3.useTexture = true;

    ROCK_4.objFile =
        asset_path(
            "models/rocks/rock5.obj"
        );

    ROCK_4.meshes =
    {
        "Rock_lp_5"
    };

    ROCK_4.textures =
    {
        &ROCK_4_TEXTURE
    };

    ROCK_4.useTexture = true;

    ROCK_5.objFile =
        asset_path(
            "models/rocks/rock6.obj"
        );

    ROCK_5.meshes =
    {
        "Rock_lp_6"
    };

    ROCK_5.textures =
    {
        &ROCK_5_TEXTURE
    };

    ROCK_5.useTexture = true;

    HUGE_ROCK.objFile =
        asset_path(
            "models/rocks/huge_rock.obj"
        );

    HUGE_ROCK.meshes =
    {
        "Plane.005"
    };

    HUGE_ROCK.textures =
    {
        nullptr
    };

    HUGE_ROCK.useTexture = false;

    // =====================================================
    //  Pássaro voador (NPC)
    //  =====================================================
    FLYING_BIRD.objFile =
        asset_path(
            "models/flying_bird/flying_bird.obj"
        );
    
    FLYING_BIRD.meshes =
    {
        "blinn2"
    };
    
    FLYING_BIRD.textures =
    {
        &FLYING_BIRD_TEXTURE
    };

    FLYING_BIRD.useTexture = true;

    // =====================================================
    //  Pato (NPC)
    //  =====================================================
    DUCK.objFile =
        asset_path(
            "models/duck/low_poly_duck.obj"
        );
    
    DUCK.meshes =
    {
        "Material.003"
    };
    
    DUCK.textures =
    {
        &DUCK_TEXTURE
    };

    DUCK.useTexture = true;
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
        &BUTTERFLY_ALBEDO_TRANSPARENCY,
        &BUTTERFLY_ALBEDO_TRANSPARENCY,
    };
    
    // =====================================================
    // CARP
    // =====================================================

    // CARP.objFile =
    //     asset_path(
    //         "models/carp/carp.obj"
    //     );
    
    // CARP.meshes = {
    //     "Mesh_0"
    // };

    // CARP.textures = {
    //     &CARP_NORMAL
    // };
    
    // CARP.useTexture = true;
}

void Assets::LoadAll() {
    LoadTextures();
    BuildModels();
}