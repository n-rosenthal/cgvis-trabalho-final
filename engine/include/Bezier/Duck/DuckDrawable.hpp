#pragma once

#include "Objects/Interfaces/Drawable.hpp"
#include <glm/glm.hpp>

class DuckDrawable : public Drawable
{
public:
    // Construtor padrão (necessário para make_unique sem parâmetros)
    DuckDrawable() = default;

    // Construtor opcional com parâmetros (se quiser inicializar pos/rot/esc)
    DuckDrawable(const glm::vec3& position,
                 const glm::vec3& rotation,
                 const glm::vec3& scale);

    // Destrutor virtual – ESSENCIAL para gerar a vtable
    virtual ~DuckDrawable() override;

    // Implementações das funções virtuais puras
    void buildMesh() override;
    void computeNormals() override;
    void setupBuffers() override;
    void draw(const DrawContext& ctx) override;
    void model(const DrawContext& ctx) override;
    void update(float dt);

private:
        float m_animationTime = 0.0f;
};