#include "Objects/LakeLayer.hpp"

/**
 * @file    `LakeLayer.cpp`
 * @brief   Camada de relevo para lagos 
 */

#include <cmath>

/**
 * @brief   Construtor padrão para camada de relevo para lagos
 * 
 * @param   center  (glm::vec2)
 *          centro do lago (x, z)
 * @param   height  (float)
 *          profundidade do lago
 * @param   radius  (float)
 *          raio do lago
 *
 * @return  Camada de relevo para lagos
 */
LakeLayer::LakeLayer(glm::vec2 center, float height, float radius) {
    m_center = center;
    m_height = height;
    m_radius = radius;
}

/**
 * @brief   Avalia a camada de relevo para lagos
 * @see     `MountainLayer`
 * 
 * @param x 
 * @param z 
 * @return float 
 */
float LakeLayer::sample(float x, float z) const{
    glm::vec2 p(x, z);

    float d = glm::distance(p, m_center);

    float t = d / m_radius;

    return
        -m_height *
        std::exp(-t * t);
}