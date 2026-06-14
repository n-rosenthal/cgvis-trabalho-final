#include <cstdio>

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

void ButterflyDrawable::model(const DrawContext& ctx) {
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

    float t = glfwGetTime();

    float flap =
        glm::radians(
            30.0f * sin(t * 8.0f)
        );

    glm::mat4 body(1.0f);

    body = glm::translate(body, m_position);
    body = glm::rotate(body, m_rotation.y, glm::vec3(0,1,0));
    body = glm::rotate(body, m_rotation.x, glm::vec3(1,0,0));
    body = glm::scale(body, m_scale);

    // corpo
    glUniformMatrix4fv(
        ctx.model_uniform,
        1,
        GL_FALSE,
        glm::value_ptr(body)
    );

    DrawVirtualObject("Butterfly_tri_torso_leg_01");

    // asa esquerda
    glm::mat4 wingL = body;
    wingL = glm::rotate(
        wingL,
        flap,
        glm::vec3(0,0,1)
    );

    glUniformMatrix4fv(
        ctx.model_uniform,
        1,
        GL_FALSE,
        glm::value_ptr(wingL)
    );

    DrawVirtualObject("wing_01");

    // asa direita
    glm::mat4 wingR = body;
    wingR = glm::rotate(
        wingR,
        -flap,
        glm::vec3(0,0,1)
    );

    glUniformMatrix4fv(
        ctx.model_uniform,
        1,
        GL_FALSE,
        glm::value_ptr(wingR)
    );

    DrawVirtualObject("wing_02");
}

void ButterflyDrawable::update(float dt) { m_animationTime += dt; };

