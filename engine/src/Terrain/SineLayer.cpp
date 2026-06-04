#include "Terrain/SineLayer.hpp"

/**
 * @file    `SineLayer.cpp`
 * @brief   Camada sinusoidal de relevo, para terrenos ondulados
 *          parametrizado em (frequência, amplitude)
 */

#include <cmath>

//  Construtor padrão
SineLayer::SineLayer(float amplitude, float frequency) {
    m_amplitude = amplitude;
    m_frequency = frequency;
}

//  Altura y = h(x, z) <=> y = amplitude * sin(x * frequency) * cos(z * frequency)
float SineLayer::sample(float x, float z) const {
    return
        m_amplitude *
        std::sin(x * m_frequency) *
        std::cos(z * m_frequency);
}