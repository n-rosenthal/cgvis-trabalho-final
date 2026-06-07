/**
 *  @file   `SceneBuilder.cpp`
 *  @brief  Implementação para a classe que constrói os objetos da cena virtual
 */

#include "Game/SceneBuilder.hpp"
#include <cstdlib>
#include <ctime>


/**
 * @brief   Constrói o terreno do jogo
 */
std::unique_ptr<Terrain> SceneBuilder::buildTerrain()
{
    auto terrain =
        std::make_unique<Terrain>(
            128,   // width
            128,   // depth
            4.0f   // spacing
        );

    terrain->generate();

    return terrain;
}

/**
 * @brief   Construção das árvores do jogo
 * @param   ptr_terrain  (Terrain&)
 *              ponteiro para o terreno
 * @return  std::vector<std::shared_ptr<Tree>>
 *              Vetor de ponteiros à árvores
 */
std::vector<std::shared_ptr<Tree>> SceneBuilder::buildTrees(Terrain& terrain) {
    std::vector<std::shared_ptr<Tree>> trees;

    glm::vec3 center(384.0f, 0.0f, 128.0f);
    glm::vec3 scale(1.0f);
    const int   numTrees = 20;
    const float dx = 12.0f, dz = 8.0f;

    for (int i = 0; i < numTrees; ++i) {
        float x = center.x - i * dx;
        float z = center.z - i * dz;

        glm::vec3 normal = terrain.getNormal(x, z);
        float     height = terrain.getHeight(x, z);

        glm::vec3 rotation(atan2(normal.z, normal.y), 0.0f, -atan2(normal.x, normal.y));
        glm::vec3 position(x, height, z);

        trees.push_back(std::make_shared<Tree>(position, rotation, scale, 0));
    }

    return trees;
}


/**
 * @brief   Construção das rochas geradas proceduralmente
 * 
 * @param   terrain     (Terrain&)
 *              ponteiro para o terreno
 * @return std::vector<std::shared_ptr<ProceduralRock>> 
 */
std::vector<std::shared_ptr<ProceduralRock>> SceneBuilder::buildRocks(Terrain& terrain) {
    std::vector<std::shared_ptr<ProceduralRock>> rocks;
    srand((unsigned int)time(nullptr));

    const int   numRocks   = 40;
    const float regionSize = 256.0f;

    for (int i = 0; i < numRocks; ++i) {
        float x     = ((float)rand() / RAND_MAX - 0.5f) * regionSize;
        float z     = ((float)rand() / RAND_MAX - 0.5f) * regionSize;
        float scale = 0.8f + ((float)rand() / RAND_MAX) * 3.5f;
        float y     = terrain.getHeight(x, z);

        rocks.push_back(std::make_shared<ProceduralRock>(glm::vec3(x, y, z), scale, 2));
    }

    return rocks;
}


/**
 * @brief   Construção dos anéis de objetivo
 * 
 * @param   terrain     (Terrain&)
 *              ponteiro para o terreno
 * @return std::vector<std::shared_ptr<Ring>> 
 */
std::vector<std::shared_ptr<Ring>> SceneBuilder::buildRings(Terrain& terrain) {
    std::vector<std::shared_ptr<Ring>> rings;

    // Posições (x, offset_y, z) — offset_y é somado à altura do terreno
    const std::vector<glm::vec3> positions = {
        { 20.0f, 20.0f,  20.0f },
        { 40.0f, 30.0f,  25.0f },
        { 60.0f, 40.0f,  30.0f },
        { 80.0f, 50.0f,  25.0f },
        {100.0f, 60.0f,  20.0f },
    };

    for (const auto& p : positions) {
        float groundY = terrain.getHeight(p.x, p.z);
        rings.push_back(std::make_shared<Ring>(glm::vec3(p.x, groundY + p.y, p.z)));
    }

    return rings;
}

/**
 *  @brief  construtor para a letter
 */
std::shared_ptr<Letter> SceneBuilder::buildLetter() {
    return std::make_shared<Letter>(glm::vec3(0.0f, 20.0f, 0.0f));
}