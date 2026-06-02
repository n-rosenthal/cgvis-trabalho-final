#pragma once

/**
 * @file    `MountainLayer.hpp`
 * @brief   Camada de relevo montanhoso 
 */

#include "TerrainLayer.hpp"

#include <glm/glm.hpp>

/**
 * @brief   Camada de relevo montanhoso
 */
class MountainLayer : public TerrainLayer {
public:
    //  Construtor padrão
    MountainLayer(glm::vec2 center, float height, float radius);

    //  altura y = h(x, z) da camada
    float sample(float x, float z) const override;
private:
    //  Centro (x, z) da montanha
    glm::vec2 m_center;

    //  Altura máxima e raio da montanha
    float m_height;
    float m_radius;
};