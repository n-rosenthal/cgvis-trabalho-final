/**
 *  @file       `SceneBuilder.hpp`
 *  @brief      Header para implementação da classe `SceneBuilder`
 *  @details    Separado de Scene para isolar a lógica de criação/configuração.
 */

#pragma once

#include <vector>
#include <memory>

#include "Terrain/Terrain.hpp"
#include "Objects/Tree.hpp"
#include "Objects/ProceduralRock.hpp"
#include "Objects/Ring.hpp"

class SceneBuilder {
public:
    std::unique_ptr<Terrain>                        buildTerrain();
    std::vector<std::shared_ptr<Tree>>              buildTrees(Terrain& terrain);
    std::vector<std::shared_ptr<ProceduralRock>>    buildRocks(Terrain& terrain);
    std::vector<std::shared_ptr<Ring>>              buildRings(Terrain& terrain);
};
