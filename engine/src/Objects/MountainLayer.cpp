#include "Objects/MountainLayer.hpp"

/**
 * @file    `MountainLayer.cpp`
 * @brief   Implementação de uma camada de relevo montanhoso
 */

#include <cmath>

/**
 * @brief Construtor padrão para camada de relevo montanhoso
 * 
 * @param center (glm::vec2)
 *          centro da montanha (x, z)
 * @param height 
 *          altura da montanha
 * @param radius 
 *          raio da montanha
 */
MountainLayer::MountainLayer(glm::vec2 center, float height, float radius) {
    m_center = center;
    m_height = height;
    m_radius = radius;
}

float MountainLayer::sample(float x, float z) const {
    // inicializa ponto (x, z) conforme parâmetros
    glm::vec2 p(x, z);

    //  calcula a distância entre o ponto e o centro da montanha
    float d = glm::distance(p, m_center);
    float t = d / m_radius;

    return m_height * std::exp(-t * t);
}