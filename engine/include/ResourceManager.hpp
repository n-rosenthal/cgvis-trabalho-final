#pragma once

#include <string>
#include <map>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "VirtualModel.h"

class ResourceManager
{
public:
    static ResourceManager& get();

    // -------------------------------------------------------------------------
    // Shaders
    // -------------------------------------------------------------------------
    void loadShaders();                      // carrega/recarrega shaders
    GLuint getProgramID() const { return m_gpuProgramID; }

    // Uniform locations
    GLint uniformModel()        const { return m_modelUniform; }
    GLint uniformView()         const { return m_viewUniform; }
    GLint uniformProjection()   const { return m_projectionUniform; }
    GLint uniformObjectId()     const { return m_objectIdUniform; }
    GLint uniformBboxMin()      const { return m_bboxMinUniform; }
    GLint uniformBboxMax()      const { return m_bboxMaxUniform; }

    // -------------------------------------------------------------------------
    // Textures
    // -------------------------------------------------------------------------
    void loadTexture(const char* filename);
    GLuint numLoadedTextures() const { return m_numLoadedTextures; }

    // -------------------------------------------------------------------------
    // Modelos OBJ (cena virtual)
    // -------------------------------------------------------------------------
    std::map<std::string, VirtualModel>& getVirtualModels() { return m_virtualModels; }
    void drawModel(const char* model_name);   // desenha um modelo pelo nome

private:
    ResourceManager();
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    // Shader helpers
    GLuint loadShader_Vertex(const char* filename);
    GLuint loadShader_Fragment(const char* filename);
    void   loadShader(const char* filename, GLuint shader_id);
    GLuint createGpuProgram(GLuint vertex_shader_id, GLuint fragment_shader_id);

    // Shader state
    GLuint m_gpuProgramID      = 0;
    GLint  m_modelUniform      = -1;
    GLint  m_viewUniform       = -1;
    GLint  m_projectionUniform = -1;
    GLint  m_objectIdUniform   = -1;
    GLint  m_bboxMinUniform    = -1;   // novo
    GLint  m_bboxMaxUniform    = -1;   // novo

    // Texture state
    GLuint m_numLoadedTextures = 0;

    // Caminhos dos shaders (construídos via ASSETS_DIR)
    std::string m_vertPath;
    std::string m_fragPath;

    // Modelos OBJ carregados (cena virtual)
    std::map<std::string, VirtualModel> m_virtualModels;
};