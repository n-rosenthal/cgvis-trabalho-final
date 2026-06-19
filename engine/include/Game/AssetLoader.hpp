#pragma once
#include "Game/Renderer.hpp"
#include "Game/Scene.hpp"
#include "Objects/Assets.hpp"
#include <map>
#include <string>
#include <memory>

enum class LoadingStep
{
    SHADERS,
    TEXTURES,
    MODELS,
    TERRAIN,
    STATIC_OBJECTS,
    DONE
};

class AssetLoader {
public:
    AssetLoader(
        Renderer& renderer,
        Scene& scene
    );

    void loadNextStep();
    bool finished() const;
    float progress() const;

private:
    Renderer& m_renderer;
    Scene&    m_scene;
    LoadingStep m_step = LoadingStep::SHADERS;
};