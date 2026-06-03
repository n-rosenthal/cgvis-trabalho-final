#pragma once

#include <glm/glm.hpp>

#include <memory>
#include <vector>

#include "TerrainType.hpp"
#include "TerrainLayer.hpp"
/**
 * @brief   Uma `TerrainRegion` é uma região de terreno/relevo do mapa
 *          sobre a qual uma ou mais camadas de relevo (`TerrainLayer`)
 *          são aplicadas.
 *  @details Uma região define:
 *
 *  - posição no mundo
 *  - raio de influência
 *  - cor/material
 *  - conjunto de camadas de relevo
 */

class TerrainRegion {
public:
    //  Construtor padrão
    TerrainRegion(
        glm::vec2 minCorner,
        glm::vec2 maxCorner,
        glm::vec3 color,
        const TerrainMaterial& material
    );

    // =========================================
    // CAMADAS
    // =========================================

    void addLayer(
        std::shared_ptr<TerrainLayer> layer
    );

    void clearLayers();

    //  =========================================
    //  Tipos e materiais de terreno
    //  =========================================
    void setColor(glm::vec3 color) { m_color = color; }
    void setMaterial(const TerrainMaterial& material) { m_material = material; }

    // =========================================
    // CONSULTAS
    // =========================================

    float getHeight(
        float x,
        float z
    ) const;

    bool contains(
        float x,
        float z
    ) const;

    glm::vec3 getColor() const;

    const TerrainMaterial& getMaterial() const;

private:
    glm::vec2 m_minCorner;
    glm::vec2 m_maxCorner;
    glm::vec3 m_color;
    TerrainMaterial m_material;

    std::vector<
        std::shared_ptr<TerrainLayer>
    > m_layers;
};