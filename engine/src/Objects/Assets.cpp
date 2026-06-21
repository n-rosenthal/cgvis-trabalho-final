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

    TextureDefinition BIRD;
    TextureDefinition BIRD_STANDING;
    TextureDefinition BIRD_STANDING_LEFT;

    TextureDefinition HOUSE_NORMAL;

    TextureDefinition MAILBOX_COLOR;

    // Árvore OAK
    TextureDefinition OAK_TEXTURE;
    TextureDefinition OAK_LEAVES_TEXTURE;

    // Arbustos
    TextureDefinition BUSH_TEXTURE;
    TextureDefinition BUXUS_TEXTURE;
    TextureDefinition SHRUB_TEXTURE;
    TextureDefinition STYLED_SHRUB_TEXTURE;

    // Planta marinha
    TextureDefinition MARINE_PLANT_TEXTURE;

    // Rochas
    TextureDefinition ROCK_1_TEXTURE;
    TextureDefinition ROCK_2_TEXTURE;
    TextureDefinition ROCK_3_TEXTURE;
    TextureDefinition ROCK_4_TEXTURE;
    TextureDefinition ROCK_5_TEXTURE;

    // NPCs
    TextureDefinition FLYING_BIRD_TEXTURE;
    TextureDefinition DUCK_TEXTURE;
    TextureDefinition BUTTERFLY_ALBEDO_TRANSPARENCY;

    // =====================================================
    // MODELOS
    // =====================================================

    ModelDefinition TREE_1;

    ModelDefinition LETTER;

    ModelDefinition BIRD_MODEL;
    ModelDefinition BIRD_STANDING_MODEL;

    ModelDefinition HOUSE;
    ModelDefinition MAILBOX;

    // Árvores
    ModelDefinition OAK;

    // Arbustos
    ModelDefinition BUSH;
    ModelDefinition BUXUS;
    ModelDefinition SHRUB;
    ModelDefinition STYLED_SHRUB;

    // Planta marinha
    ModelDefinition MARINE_PLANT;

    // Rochas
    ModelDefinition ROCK_1;
    ModelDefinition ROCK_2;
    ModelDefinition ROCK_3;
    ModelDefinition ROCK_4;
    ModelDefinition ROCK_5;
    ModelDefinition ROCK_6;
    ModelDefinition ROCK_7;
    ModelDefinition ROCK_8;
    ModelDefinition ROCK_9;

    // NPCs
    ModelDefinition FLYING_BIRD;
    ModelDefinition DUCK;
    ModelDefinition BUTTERFLY;

}

static ModelDefinition makeModel(
    const std::string& obj,
    std::initializer_list<const char*> meshes,
    std::initializer_list<TextureDefinition*> textures,
    bool alpha = false,
    bool useTex = true
)
{
    ModelDefinition model;

    model.objFile = obj;

    model.meshes.assign(
        meshes.begin(),
        meshes.end()
    );

    model.textures.assign(
        textures.begin(),
        textures.end()
    );

    model.alphaCutout = alpha;
    model.useTexture  = useTex;

    return model;
}

static TextureDefinition makeTexture(
    const std::string& file,
    bool repeat = true
)
{
    TextureDefinition tex;

    tex.file = file;
    tex.id = LoadTextureImage(
        tex.file.c_str(),
        repeat
    );

    return tex;
}

static TextureDefinition makeTextureAsset(
    const char* relativePath,
    bool repeat = true
)
{
    return makeTexture(
        asset_path(relativePath),
        repeat
    );
}

void Assets::LoadTextures()
{
    // =====================================================
    // TREE 1
    // =====================================================

    TREE_1_TRUNK =
        makeTextureAsset(
            "textures/tree1_textures/Tree_1_Trunk_Limbs-DIFFUSE.png"
        );

    TREE_1_TWIGS =
        makeTextureAsset(
            "textures/tree1_textures/Tree_1_Twigs-DIFFUSE.jpg"
        );

    TREE_1_LEAVES =
        makeTextureAsset(
            "textures/tree1_textures/Tree_1_Leaf.png"
        );

    // =====================================================
    // BIRD
    // =====================================================

    BIRD =
        makeTextureAsset(
            "models/bird/falcon2.jpg"
        );

    BIRD_STANDING =
        makeTextureAsset(
            "models/bird_standing/default-grey.jpg"
        );

    BIRD_STANDING_LEFT =
        makeTextureAsset(
            "models/bird_standing/bird1l.jpg"
        );

    // =====================================================
    // HOUSE
    // =====================================================

    HOUSE_NORMAL =
        makeTextureAsset(
            "textures/house/Material__23_baseColor.jpeg"
        );

    // =====================================================
    // MAILBOX
    // =====================================================

    MAILBOX_COLOR =
        makeTextureAsset(
            "textures/mailbox/M_Mailbox_baseColor.png"
        );

    // =====================================================
    // OAK
    // =====================================================

    OAK_TEXTURE =
        makeTextureAsset(
            "textures/tree_oak/Cube_001_diffuse.png"
        );

    OAK_LEAVES_TEXTURE =
        makeTextureAsset(
            "textures/tree_oak/Plane_067_diffuse.png"
        );

    // =====================================================
    // BUSH
    // =====================================================

    BUSH_TEXTURE =
        makeTextureAsset(
            "textures/bush/vetka_kusta_0.png",
            false
        );

    // =====================================================
    // BUXUS
    // =====================================================

    BUXUS_TEXTURE =
        makeTextureAsset(
            "textures/buxus/Buxus_Base_color.png"
        );

    // =====================================================
    // SHRUB
    // =====================================================

    SHRUB_TEXTURE =
        makeTextureAsset(
            "textures/low_poly_shrub/bush_PNG7214.png"
        );

    // =====================================================
    // STYLED SHRUB
    // =====================================================

    STYLED_SHRUB_TEXTURE =
        makeTextureAsset(
            "textures/styled_bush/9.png"
        );

    // =====================================================
    // MARINE PLANT
    // =====================================================

    MARINE_PLANT_TEXTURE =
        makeTextureAsset(
            "textures/marine_plant/plant_1_diffuse.jpeg"
        );

    // =====================================================
    // ROCKS
    // =====================================================

    ROCK_1_TEXTURE =
        makeTextureAsset(
            "textures/rocks/rock1_lp_1001_BaseColor.png"
        );

    ROCK_2_TEXTURE =
        makeTextureAsset(
            "textures/rocks/rock3_lp_1001_BaseColor.png"
        );

    ROCK_3_TEXTURE =
        makeTextureAsset(
            "textures/rocks/rock4_lp_1001_BaseColor.png"
        );

    ROCK_4_TEXTURE =
        makeTextureAsset(
            "textures/rocks/rock5_lp_1001_BaseColor.png"
        );

    ROCK_5_TEXTURE =
        makeTextureAsset(
            "textures/rocks/rock6_lp_1001_BaseColor.png"
        );

    // =====================================================
    // FLYING BIRD
    // =====================================================

    FLYING_BIRD_TEXTURE =
        makeTextureAsset(
            "textures/flying_bird/BirdUVTexture.jpeg"
        );

    // =====================================================
    // DUCK
    // =====================================================

    DUCK_TEXTURE =
        makeTextureAsset(
            "textures/low_poly_duck/duck skin _0.png"
        );

    // =====================================================
    // BUTTERFLY
    // =====================================================

    BUTTERFLY_ALBEDO_TRANSPARENCY =
        makeTextureAsset(
            "textures/butterfly/butt_low_lambert1_AlbedoTransparency.png"
        );
}

void Assets::BuildModels()
{
    TREE_1 = makeModel(
        asset_path(
            "models/trees/GenTree-103_AE3D_03122023-F1.obj"
        ),
        {
            "leaves_Material-Leaves",
            "GenTree-Main_Trunk_Material.Trunk_and_Primary_Limbs",
            "GenTree_-_Twigs_Leaf_Bearing_Material.Twigs"
        },
        {
            &TREE_1_LEAVES,
            &TREE_1_TRUNK,
            &TREE_1_TWIGS
        },
        true
    );

    BIRD_MODEL = makeModel(
        asset_path(
            "models/bird/0V3HJRW3DQ5QPF3J2O5PR4Z1M.obj"
        ),
        {
            "the_bird"
        },
        {
            &BIRD
        }
    );

    BIRD_STANDING_MODEL = makeModel(
        asset_path(
            "models/bird_standing/G1FXKUZIFSQHX0QERXO6AAO63.obj"
        ),
        {
            "the_bird2",
            "Object_color_0.031360-0.009440-0.009440.jpg"
        },
        {
            &BIRD_STANDING_LEFT,
            &BIRD_STANDING
        }
    );

    LETTER = makeModel(
        asset_path(
            "models/the_letter.obj"
        ),
        {
            "the_letter"
        },
        {}
    );

    HOUSE = makeModel(
        asset_path(
            "models/house/house.obj"
        ),
        {
            "Object_3"
        },
        {
            &HOUSE_NORMAL
        }
    );

    MAILBOX = makeModel(
        asset_path(
            "models/mailbox/mailbox.obj"
        ),
        {
            "Object_4"
        },
        {
            &MAILBOX_COLOR
        }
    );

    OAK = makeModel(
        asset_path(
            "models/tree_oak/tree_oak.obj"
        ),
        {
            "Cube_001_2_0",
            "Plane_067"
        },
        {
            &OAK_TEXTURE,
            &OAK_LEAVES_TEXTURE
        }
    );

    BUSH = makeModel(
        asset_path(
            "models/bush/bush.obj"
        ),
        {
            "vetka_kusta.004"
        },
        {
            &BUSH_TEXTURE
        }
    );

    BUXUS = makeModel(
        asset_path(
            "models/buxus/buxus.obj"
        ),
        {
            "Bush_low"
        },
        {
            &BUXUS_TEXTURE
        }
    );

    SHRUB = makeModel(
        asset_path(
            "models/low_poly_shrub/low_poly_shrub.obj"
        ),
        {
            "pPlane5",
            "pPlane6",
            "pPlane7",
            "pPlane8"
        },
        {
            &SHRUB_TEXTURE
        }
    );

    STYLED_SHRUB = makeModel(
        asset_path(
            "models/styled bush/styled_bush.obj"
        ),
        {
            "stylized_bush"
        },
        {
            &STYLED_SHRUB_TEXTURE
        }
    );

    MARINE_PLANT = makeModel(
        asset_path(
            "models/marine_plant/marine_plant.obj"
        ),
        {
            "plant"
        },
        {
            &MARINE_PLANT_TEXTURE
        }
    );

    ROCK_1 = makeModel(
        asset_path(
            "models/rocks/rock1.obj"
        ),
        {
            "Rock_lp_1"
        },
        {
            &ROCK_1_TEXTURE
        }
    );

    ROCK_2 = makeModel(
        asset_path(
            "models/rocks/rock3.obj"
        ),
        {
            "Rock_lp_3"
        },
        {
            &ROCK_2_TEXTURE
        }
    );

    ROCK_3 = makeModel(
        asset_path(
            "models/rocks/rock4.obj"
        ),
        {
            "Rock_lp_4"
        },
        {
            &ROCK_3_TEXTURE
        }
    );

    ROCK_4 = makeModel(
        asset_path(
            "models/rocks/rock5.obj"
        ),
        {
            "Rock_lp_5"
        },
        {
            &ROCK_4_TEXTURE
        }
    );

    ROCK_5 = makeModel(
        asset_path(
            "models/rocks/rock6.obj"
        ),
        {
            "Rock_lp_6"
        },
        {
            &ROCK_5_TEXTURE
        }
    );

    //  rochas que talvez funcionem
    ROCK_6 = makeModel(
        asset_path(
            "models/rocks/ROCK_2.obj"
        ),
        {
            "Object_34"
        },
        {
            &ROCK_1_TEXTURE // note!
        }
    );

    ROCK_7 = makeModel(
        asset_path(
            "models/rocks/ROCK_4.obj"
        ),
        {
            "Object_26"
        },
        {
            &ROCK_2_TEXTURE // note!
        }
    );

    ROCK_8 = makeModel(
        asset_path(
            "models/rocks/ROCK_6.obj"
        ),
        {
            "Object_18"
        },
        {
            &ROCK_3_TEXTURE // note!
        }
    );

    ROCK_9 = makeModel(
        asset_path(
            "models/rocks/ROCK_8.obj"
        ),
        {
            "Object_4"
        },
        {
            &ROCK_4_TEXTURE // note!
        }
    );





    FLYING_BIRD = makeModel(
        asset_path(
            "models/flying_bird/flying_bird.obj"
        ),
        {
            "blinn2"
        },
        {
            &FLYING_BIRD_TEXTURE
        }
    );

    DUCK = makeModel(
        asset_path(
            "models/duck/low_poly_duck.obj"
        ),
        {
            "Material.003"
        },
        {
            &DUCK_TEXTURE
        }
    );

    BUTTERFLY = makeModel(
        asset_path(
            "models/butterfly/butterfly.obj"
        ),
        {
            "Butterfly_tri_torso_leg_01",
            "wing_01",
            "wing_02"
        },
        {
            &BUTTERFLY_ALBEDO_TRANSPARENCY,
            &BUTTERFLY_ALBEDO_TRANSPARENCY,
            &BUTTERFLY_ALBEDO_TRANSPARENCY
        },
        true
    );
}

void Assets::LoadAll() {
    LoadTextures();
    BuildModels();
}