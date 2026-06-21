#include <cstdio>
#include "matrices.h"

#include "Game/Renderer.hpp"
#include "Bezier/Butterfly/ButterflyDrawable.hpp"
#include "Objects/Assets.hpp"

extern void DrawVirtualObject(const char* name);


/**
 * @brief   Função vazia pois, uma vez que queremos movimentar as asas da borboleta
 *          faremos o draw() em model(); normalmente, model() chama draw() ao final
 *          neste caso, iremos reescrever model() para a subclasse específica.
 * 
 * @param ctx 
 */
void ButterflyDrawable::draw(
    const DrawContext& ctx
) {};

void ButterflyDrawable::model(const DrawContext& ctx)
{
    glUniform1i(
        ctx.object_id_uniform,
        Renderer::OBJ_BUTTERFLY
    );

    GLint useTextureLoc =
        glGetUniformLocation(
            ctx.shader_program,
            "useTexture"
        );

    GLint texLoc =
        glGetUniformLocation(
            ctx.shader_program,
            "diffuseTexture"
        );

    glUniform1i(useTextureLoc, 1);

    glActiveTexture(GL_TEXTURE0);

    glBindTexture(
        GL_TEXTURE_2D,
        Assets::BUTTERFLY_ALBEDO_TRANSPARENCY.id
    );

    glUniform1i(texLoc, 0);

    // --------------------------------------------------------
    // Animação das asas
    // --------------------------------------------------------

    float t = glfwGetTime();

    float flap =
        glm::radians(
            30.0f * sin(8.0f * t)
        );

    // --------------------------------------------------------
    // Matriz base da borboleta
    // Equivalente a:
    //
    // body = translate(...)
    // body = rotateY(...)
    // body = rotateX(...)
    // body = scale(...)
    // --------------------------------------------------------

    glm::mat4 body =
        Matrix_Translate(
            m_position.x,
            m_position.y,
            m_position.z
        )
        *
        Matrix_Rotate_Y(
            m_rotation.y
        )
        *
        Matrix_Rotate_X(
            m_rotation.x
        )
        *
        Matrix_Scale(
            m_scale.x,
            m_scale.y,
            m_scale.z
        );

    // --------------------------------------------------------
    // Corpo
    // --------------------------------------------------------

    glUniformMatrix4fv(
        ctx.model_uniform,
        1,
        GL_FALSE,
        glm::value_ptr(body)
    );

    DrawVirtualObject(
        "Butterfly_tri_torso_leg_01"
    );

    // --------------------------------------------------------
    // Asa esquerda
    // --------------------------------------------------------

    glm::mat4 wingL =
        body *
        Matrix_Rotate_Y(
            flap
        );

    glUniformMatrix4fv(
        ctx.model_uniform,
        1,
        GL_FALSE,
        glm::value_ptr(wingL)
    );

    DrawVirtualObject(
        "wing_01"
    );

    // --------------------------------------------------------
    // Asa direita
    // --------------------------------------------------------

    glm::mat4 wingR =
        body *
        Matrix_Rotate_Y(
            -flap
        );

    glUniformMatrix4fv(
        ctx.model_uniform,
        1,
        GL_FALSE,
        glm::value_ptr(wingR)
    );

    DrawVirtualObject(
        "wing_02"
    );
}

void ButterflyDrawable::update(float dt) { m_animationTime += dt; };

