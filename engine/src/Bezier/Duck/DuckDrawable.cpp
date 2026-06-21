#include "Bezier/Duck/DuckDrawable.hpp"
#include "Game/Renderer.hpp"
#include "Objects/Assets.hpp"
#include "matrices.h"
#include <glm/gtc/type_ptr.hpp>

extern void DrawVirtualObject(const char* name);

// Construtor com parâmetros (se usado)
DuckDrawable::DuckDrawable(const glm::vec3& position,
                           const glm::vec3& rotation,
                           const glm::vec3& scale)
    : Drawable()   // chama o construtor padrão da base (não faz nada)
{
    m_position = position;
    m_rotation = rotation;
    m_scale    = scale;
}

// Destrutor virtual – definição vazia, mas necessária
DuckDrawable::~DuckDrawable() = default;

// ------------------------------------------------------------
// Métodos obrigatórios da interface Drawable (não usados aqui,
// pois o modelo é carregado via DrawVirtualObject, mas precisam existir)
// ------------------------------------------------------------
void DuckDrawable::buildMesh()
{
    // Não precisa construir malha, pois usamos modelo OBJ externo
}

void DuckDrawable::computeNormals()
{
    // Idem
}

void DuckDrawable::setupBuffers()
{
    // Idem
}

// ------------------------------------------------------------
// draw() vazio – a renderização é feita em model()
// ------------------------------------------------------------
void DuckDrawable::draw(const DrawContext& ctx)
{
    // Nada
}

// ------------------------------------------------------------
// model() – faz a transformação e desenha o modelo OBJ
// ------------------------------------------------------------
void DuckDrawable::model(const DrawContext& ctx)
{
    // Define o identificador do objeto
    glUniform1i(ctx.object_id_uniform, Renderer::OBJ_DUCK);

    // Configura a textura
    GLint useTextureLoc = glGetUniformLocation(ctx.shader_program, "useTexture");
    GLint texLoc        = glGetUniformLocation(ctx.shader_program, "diffuseTexture");

    glUniform1i(useTextureLoc, 1);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Assets::DUCK_TEXTURE.id);
    glUniform1i(texLoc, 0);

    // Matriz de transformação
    glm::mat4 modelMat =
        Matrix_Translate(m_position.x, m_position.y, m_position.z) *
        Matrix_Rotate_Y(m_rotation.y) *
        Matrix_Rotate_X(m_rotation.x) *
        Matrix_Scale(m_scale.x, m_scale.y, m_scale.z);

    glUniformMatrix4fv(ctx.model_uniform, 1, GL_FALSE, glm::value_ptr(modelMat));

    // Desenha o modelo
    DrawVirtualObject("duck");
}

// ------------------------------------------------------------
// update() – apenas para manter animação (não usada)
// ------------------------------------------------------------
void DuckDrawable::update(float dt)
{
    m_animationTime += dt;   // se você tiver essa variável
}