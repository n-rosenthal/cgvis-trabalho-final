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

    extern TextureDefinition TREE_2_TRUNK;
    extern TextureDefinition TREE_2_TWIGS;
    extern TextureDefinition TREE_2_LEAVES;

    extern TextureDefinition RED_BRICKS;
    extern TextureDefinition ROCKY_TERRAIN;

    extern TextureDefinition BIRD;
    extern TextureDefinition BIRD_STANDING;
    extern TextureDefinition BIRD_STANDING_LEFT;

    extern TextureDefinition BUTTERFLY_ALBEDO_TRANSPARENCY;
    extern TextureDefinition BUTTERFLY_METTALLIC_SMOOTHNENESS;
    extern TextureDefinition BUTTERFLY_NORMAL;

    // =====================================================
    // MODELOS
    // =====================================================

    extern ModelDefinition TREE_1;
    extern ModelDefinition TREE_2;

    extern ModelDefinition LETTER;

    extern ModelDefinition BIRD_MODEL;
    extern ModelDefinition BIRD_STANDING_MODEL;

    extern ModelDefinition HOUSE;

    extern ModelDefinition BUTTERFLY;

    // =====================================================
    // CICLO DE VIDA
    // =====================================================

    void LoadTextures();
    void BuildModels();
    void LoadAll();
}