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

    for(size_t i = 0; i < m_model->meshes.size(); ++i) {
        // printf("i=%zu\n", i);

        // printf(
        //     "mesh_count=%zu texture_count=%zu\n",
        //     m_model->meshes.size(),
        //     m_model->textures.size()
        // );

        fflush(stdout);

        const std::string& meshName =
            m_model->meshes.at(i);

        // printf(
        //     "mesh=%s\n",
        //     meshName.c_str()
        // );

        fflush(stdout);

        bool hasTexture =
            i < m_model->textures.size() &&
            m_model->textures[i] != nullptr;

        // printf(
        //     "hasTexture=%d\n",
        //     (int)hasTexture
        // );

        fflush(stdout);

        if(hasTexture)
        {
            // printf(
            //     "texture id=%u\n",
            //     m_model->textures[i]->id
            // );

            fflush(stdout);

            glActiveTexture(GL_TEXTURE0);

            glBindTexture(
                GL_TEXTURE_2D,
                m_model->textures[i]->id
            );

            glUniform1i(texLoc, 0);
        }

        DrawVirtualObject(
            meshName.c_str()
        );
    }
}