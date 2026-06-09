#include "Objects/ObjDrawable.hpp"
#include <glad/glad.h>

ObjDrawable::ObjDrawable(
    const ModelDefinition& model
)
:
m_model(&model)
{
}

void ObjDrawable::draw(const DrawContext& ctx){
    // qual textura?
    GLint useTextureLoc =
        glGetUniformLocation(
            ctx.shader_program,
            "useTexture"
        );
    
    glUniform1i(
        useTextureLoc,
        m_model->useTexture ? 1 : 0
    );
    
    // qual textura?
    GLint texLoc =
        glGetUniformLocation(
            ctx.shader_program,
            "diffuseTexture"
        );

    for(size_t i = 0; i < m_model->meshes.size(); ++i)
    {
        bool hasTexture =
            i < m_model->textures.size() &&
            m_model->textures[i] != nullptr;

        glUniform1i(
            useTextureLoc,
            hasTexture ? 1 : 0
        );

        if(hasTexture)
        {
            glActiveTexture(GL_TEXTURE0);

            glBindTexture(
                GL_TEXTURE_2D,
                m_model->textures[i]->id
            );

            glUniform1i(texLoc, 0);
        }

        DrawVirtualObject(
            m_model->meshes[i].c_str()
        );
    }
}