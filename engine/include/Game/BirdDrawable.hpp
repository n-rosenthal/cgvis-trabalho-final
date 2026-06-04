#pragma once
#include "Objects/Interfaces/Drawable.hpp"
#include "Loaders/ObjLoader.hpp"

class BirdDrawable : public Drawable {
public:
    void buildMesh() override {}      // modelo já carregado globalmente
    void computeNormals() override {} // já possui normais
    void setupBuffers() override {}   // buffers já configurados
    void draw(const DrawContext& /*ctx*/) override {
        DrawVirtualObject("the_bird");
    }
};