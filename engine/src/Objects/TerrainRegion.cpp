#include <glm/geometric.hpp>

#include "Objects/TerrainRegion.hpp"
#include "Objects/TerrainType.hpp"

TerrainRegion::TerrainRegion(
    glm::vec2 minCorner,
    glm::vec2 maxCorner,
    glm::vec3 color,
    const TerrainMaterial& material) {

    m_minCorner = minCorner;
    m_maxCorner = maxCorner;
    m_color     = color;
    m_material  = material;
}

void TerrainRegion::addLayer(
    std::shared_ptr<TerrainLayer> layer
)
{
    m_layers.push_back(layer);
}

void TerrainRegion::clearLayers()
{
    m_layers.clear();
}

bool TerrainRegion::contains(float x, float z) const {
    return
        x >= m_minCorner.x &&
        x <= m_maxCorner.x &&
        z >= m_minCorner.y &&
        z <= m_maxCorner.y;
}

float TerrainRegion::getHeight(float x, float z) const {
    float height = 0.0f;

    for (auto& layer : m_layers) {
        height +=
            layer->sample(x, z);
    }

    return height;
}

glm::vec3 TerrainRegion::getColor() const
{
    return m_color;
}

const TerrainMaterial& TerrainRegion::getMaterial() const
{
    return m_material;
}