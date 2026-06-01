#include "TerrainType.hpp"

namespace Materials {
    static TerrainMaterial Grass =
    {
        glm::vec3(0.2f, 0.7f, 0.2f),

        0.25f,
        0.10f,
        8.0f,

        TerrainType::Grass
    };

    static TerrainMaterial Rock =
    {
        glm::vec3(0.5f, 0.5f, 0.5f),

        0.25f,
        0.35f,
        32.0f,

        TerrainType::Rock
    };

    static TerrainMaterial Sand =
    {
        glm::vec3(0.9f, 0.8f, 0.5f),

        0.25f,
        0.15f,
        8.0f,

        TerrainType::Sand
    };

    static TerrainMaterial Snow =
    {
        glm::vec3(0.95f, 0.95f, 0.95f),

        0.35f,
        0.60f,
        64.0f,

        TerrainType::Snow
    };

    static TerrainMaterial Water =
    {
        glm::vec3(0.1f, 0.3f, 0.8f),

        0.20f,
        1.00f,
        128.0f,

        TerrainType::Water
    };
}