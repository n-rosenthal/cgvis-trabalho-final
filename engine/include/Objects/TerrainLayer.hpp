#pragma once

/**
 * @brief Interface para camadas de relevo.
 *
 * Cada camada recebe uma coordenada (x,z)
 * e retorna uma contribuição para a altura.
 */
class TerrainLayer
{
public:
    //  Destruidor padrão
    virtual ~TerrainLayer() = default;

    /**
     * @brief Avalia a camada em uma posição.
     */
    virtual float sample(
        float x,
        float z
    ) const = 0;
};