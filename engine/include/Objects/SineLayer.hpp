#pragma once

#include "TerrainLayer.hpp"
/**
 * @brief   Camada de relevo sinusoidal
 */
class SineLayer : public TerrainLayer {
public:
    SineLayer(float amplitude, float frequency);

    //  altura y = h(x, z) da camada sinusoidal
    float sample(float x, float z) const override;

private:
    //  amplitude e frequência da onda sinusoidal
    float m_amplitude;
    float m_frequency;
};