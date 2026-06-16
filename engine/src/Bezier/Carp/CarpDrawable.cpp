/**
 *  @file   `CarpDrawable.cpp`
 */


#include <cstdio>

#include "Game/Renderer.hpp"
#include "Bezier/Carp/CarpDrawable.hpp"
#include "Objects/Assets.hpp"

extern void DrawVirtualObject(const char* name);

/**
 * @brief   Desenha o NPC carpa
 */
void CarpDrawable::draw(const DrawContext&) {
    DrawVirtualObject(
        "Mesh_0"
    );
};

void CarpDrawable::model(
    const DrawContext& ctx
)
{
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

    glUniform1i(
        useTextureLoc,
        1
    );

    glActiveTexture(
        GL_TEXTURE0
    );

    glBindTexture(
        GL_TEXTURE_2D,
        Assets::CARP_NORMAL.id
    );

    glUniform1i(
        texLoc,
        0
    );

    float t = glfwGetTime();

    float sway =
        glm::radians(
            12.0f *
            sin(
                t * 4.0f
            )
        );

    float bob =
        0.25f *
        sin(
            t * 2.0f
        );

    glm::mat4 m(1.0f);

    m =
        glm::translate(
            m,
            m_position +
            glm::vec3(
                0.0f,
                bob,
                0.0f
            )
        );

    m =
        glm::rotate(
            m,
            m_rotation.y + sway,
            glm::vec3(
                0,1,0
            )
        );

    m =
        glm::rotate(
            m,
            m_rotation.x,
            glm::vec3(
                1,0,0
            )
        );

    m =
        glm::scale(
            m,
            m_scale
        );

    glUniformMatrix4fv(
        ctx.model_uniform,
        1,
        GL_FALSE,
        glm::value_ptr(m)
    );

    glUniform1i(
        ctx.object_id_uniform,
        ctx.object_id
    );

    DrawVirtualObject(
        "Mesh_0"
    );
}