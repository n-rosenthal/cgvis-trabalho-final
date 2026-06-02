#pragma once

#include <glm/glm.hpp>

/**
 * @brief   Tipos de terreno
 */
enum class TerrainType {
    Grass,
    Rock,
    Sand,
    Snow,
    Water
};

/**
 * @brief   Materiais de terreno
 */
struct TerrainMaterial {
    glm::vec3 diffuseColor;

    float ambientStrength;
    float specularStrength;
    float shininess;

    TerrainType type;
    int id;
};