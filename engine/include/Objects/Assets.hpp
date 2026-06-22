#pragma once
/**
 * @file    `Assets.hpp`
 * @brief   Simplificação. Reúne em si modelos, texturas e caminhos para
 *          modelos OBJ.
 */

#include "glad/glad.h"
#include <string>
#include <vector>

struct TextureDefinition {
    //  ID da textura
    GLuint      id = 0;

    //  caminho da textura
    std::string file;
};

/**
  * @brief      Definição de um modelo.
  * @details    Um modelo é definido por
  *                 seu arquivo OBJ
  *                 suas texturas
  *                 e seus meshes (malhas de triângulos)
  */
struct ModelDefinition {
    std::string objFile;
    std::vector<std::string> meshes;
    std::vector<TextureDefinition*> textures;
    bool alphaCutout = false;
    bool useTexture  = true;
};

/**
 * @brief       Registro dos modelos utilizados pelo jogo
 */
namespace Assets {
    // =====================================================
    // TEXTURAS
    // =====================================================
    extern TextureDefinition TREE_1_TRUNK;
    extern TextureDefinition TREE_1_TWIGS;
    extern TextureDefinition TREE_1_LEAVES;

    extern TextureDefinition BIRD;
    extern TextureDefinition BIRD_STANDING;
    extern TextureDefinition BIRD_STANDING_LEFT;

    extern TextureDefinition HOUSE_NORMAL;

    extern TextureDefinition MAILBOX_COLOR;
    
    //  Árvore OAK
    extern TextureDefinition OAK_TEXTURE;
    extern TextureDefinition OAK_LEAVES_TEXTURE;


    //  Arbusto SHRUB
    extern TextureDefinition SHRUB_TEXTURE;

    //  Arbusto STYLED_SHRUB
    extern TextureDefinition STYLED_SHRUB_TEXTURE;

    //  Planta MARINE
    extern TextureDefinition MARINE_PLANT_TEXTURE;

    //  Flor MARGARIDA
    //  sem textura?

    //  ROCHAS (1-5, HUGE_ROCK)
    extern TextureDefinition ROCK_1_TEXTURE;
    extern TextureDefinition ROCK_2_TEXTURE;
    extern TextureDefinition ROCK_3_TEXTURE;
    extern TextureDefinition ROCK_4_TEXTURE;
    extern TextureDefinition ROCK_5_TEXTURE;

    //  Pato (NPC)
    extern TextureDefinition DUCK_TEXTURE;

    //  NPCs
    extern TextureDefinition BUTTERFLY_ALBEDO_TRANSPARENCY;

    // =====================================================
    // MODELOS
    // =====================================================

    extern ModelDefinition TREE_1;

    extern ModelDefinition LETTER;

    extern ModelDefinition BIRD_MODEL;
    extern ModelDefinition BIRD_STANDING_MODEL;

    extern ModelDefinition HOUSE;
    extern ModelDefinition MAILBOX;


    //  Árvore OAK
    extern ModelDefinition OAK;


    //  Arbusto SHRUB
    extern ModelDefinition SHRUB;

    //  Arbusto STYLED_SHRUB
    extern ModelDefinition STYLED_SHRUB;

    //  Planta MARINE
    extern ModelDefinition MARINE_PLANT;

    //  ROCHAS (1-5)
    extern ModelDefinition ROCK_1;
    extern ModelDefinition ROCK_2;
    extern ModelDefinition ROCK_3;
    extern ModelDefinition ROCK_4;
    extern ModelDefinition ROCK_5;
    extern ModelDefinition ROCK_6;
    extern ModelDefinition ROCK_7;
    extern ModelDefinition ROCK_8;
    extern ModelDefinition ROCK_9;

    //  Pato (NPC)
    extern ModelDefinition DUCK;

    //  NPCs
    extern ModelDefinition BUTTERFLY;

    // =====================================================
    // CICLO DE VIDA
    // =====================================================

    void LoadTextures();
    void BuildModels();
    void LoadAll();
}