/**
 * @file    BirdDrawable.hpp
 * @brief   Drawable para o pássaro controlado pelo jogador.
 *          Delega o draw para o modelo OBJ carregado na cena virtual,
 *          idêntico ao padrão do TreeDrawable.
 */
#pragma once

#include "Objects/Interfaces/Drawable.hpp"

class BirdDrawable : public Drawable {
public:
    BirdDrawable() = default;

    // Geometria vem do OBJ — nada a gerar em tempo de execução
    void buildMesh()      override {}
    void computeNormals() override {}
    void setupBuffers()   override {}

protected:
    void draw(const DrawContext& ctx) override;
};