#pragma once

/**
 * @file    `LakeLayer.hpp`
 * @brief   Camada de relevo negativo para lagos
 */

#include "TerrainLayer.hpp"

#include <glm/glm.hpp>

/**
 * @brief   Camada de relevo para lagos
 */
class LakeLayer: public TerrainLayer {
    public:
        //  Construtor padrão
        LakeLayer(glm::vec2 center, float height, float radius);

        //  altura y = h(x, z) da camada
        float sample(float x, float z) const override;

    private:
        //  Centro (x, z) do lago
        glm::vec2 m_center;

        //  profundidade e raio do lago
        float m_height;
        float m_radius;
};