// ResourceManager.cpp
// Implementation of ResourceManager – shaders and textures only.
// (No virtual scene handling – that is now in ObjectManager from Objects.h)

#include "ResourceManager.hpp"

#include <cstdio>      // printf, fprintf
#include <cstdlib>     // std::exit
#include <fstream>     // std::ifstream
#include <sstream>     // std::stringstream
#include <string>

#include <glad/glad.h>
#include <stb_image.h>


// ---------------------------------------------------------------------------
// Constructor – builds full paths to shaders using ASSETS_DIR macro
// (ASSETS_DIR is defined by CMake via target_compile_definitions)
// ---------------------------------------------------------------------------
ResourceManager::ResourceManager()
    : m_vertPath(std::string(ASSETS_DIR) + "/shaders/shader_vertex.glsl")
    , m_fragPath(std::string(ASSETS_DIR) + "/shaders/shader_fragment.glsl")
{}

// ---------------------------------------------------------------------------
// Singleton access
// ---------------------------------------------------------------------------
ResourceManager& ResourceManager::get()
{
    static ResourceManager instance;
    return instance;
}

// ---------------------------------------------------------------------------
// loadShaders – loads vertex and fragment shaders, creates GPU program,
// and caches uniform locations.
// ---------------------------------------------------------------------------
void ResourceManager::loadShaders()
{
    GLuint vertex_shader_id   = loadShader_Vertex(m_vertPath.c_str());
    GLuint fragment_shader_id = loadShader_Fragment(m_fragPath.c_str());

    // Delete previous program if it exists (supports hot‑reload with 'R')
    if (m_gpuProgramID != 0)
        glDeleteProgram(m_gpuProgramID);

    m_gpuProgramID = createGpuProgram(vertex_shader_id, fragment_shader_id);

    m_bboxMinUniform = glGetUniformLocation(m_gpuProgramID, "bbox_min");
    m_bboxMaxUniform = glGetUniformLocation(m_gpuProgramID, "bbox_max");

    // Query uniform locations
    m_modelUniform      = glGetUniformLocation(m_gpuProgramID, "model");
    m_viewUniform       = glGetUniformLocation(m_gpuProgramID, "view");
    m_projectionUniform = glGetUniformLocation(m_gpuProgramID, "projection");
    m_objectIdUniform   = glGetUniformLocation(m_gpuProgramID, "object_id");

    // Bind texture units to the fragment shader samplers
    glUseProgram(m_gpuProgramID);
    glUniform1i(glGetUniformLocation(m_gpuProgramID, "TextureImage0"), 0);
    glUniform1i(glGetUniformLocation(m_gpuProgramID, "TextureImage1"), 1);
    glUniform1i(glGetUniformLocation(m_gpuProgramID, "TextureImage2"), 2);
    glUseProgram(0);
}

// ---------------------------------------------------------------------------
// loadTexture – loads an image file and creates an OpenGL texture object.
// ---------------------------------------------------------------------------
void ResourceManager::loadTexture(const char* filename)
{
    printf("Carregando imagem \"%s\"... ", filename);

    stbi_set_flip_vertically_on_load(true);
    int width, height, channels;
    unsigned char* data = stbi_load(filename, &width, &height, &channels, 3);

    if (data == NULL)
    {
        fprintf(stderr, "ERROR: Cannot open image file \"%s\".\n", filename);
        std::exit(EXIT_FAILURE);
    }

    printf("OK (%dx%d).\n", width, height);

    GLuint texture_id;
    GLuint sampler_id;
    glGenTextures(1, &texture_id);
    glGenSamplers(1, &sampler_id);

    glSamplerParameteri(sampler_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(sampler_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(sampler_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glSamplerParameteri(sampler_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glPixelStorei(GL_UNPACK_ALIGNMENT,  1);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
    glPixelStorei(GL_UNPACK_SKIP_ROWS,   0);

    GLuint textureunit = m_numLoadedTextures;
    glActiveTexture(GL_TEXTURE0 + textureunit);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8, width, height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindSampler(textureunit, sampler_id);

    stbi_image_free(data);
    m_numLoadedTextures += 1;
}

// ---------------------------------------------------------------------------
// Private shader loading helpers (identical to original LoadShader_*)
// ---------------------------------------------------------------------------
GLuint ResourceManager::loadShader_Vertex(const char* filename)
{
    GLuint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
    loadShader(filename, vertex_shader_id);
    return vertex_shader_id;
}

GLuint ResourceManager::loadShader_Fragment(const char* filename)
{
    GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
    loadShader(filename, fragment_shader_id);
    return fragment_shader_id;
}

void ResourceManager::loadShader(const char* filename, GLuint shader_id)
{
    std::ifstream file;
    try {
        file.exceptions(std::ifstream::failbit);
        file.open(filename);
    } catch (std::exception&) {
        fprintf(stderr, "ERROR: Cannot open file \"%s\".\n", filename);
        std::exit(EXIT_FAILURE);
    }

    std::stringstream ss;
    ss << file.rdbuf();
    std::string str = ss.str();
    const GLchar* shader_string        = str.c_str();
    const GLint   shader_string_length = static_cast<GLint>(str.length());

    glShaderSource(shader_id, 1, &shader_string, &shader_string_length);
    glCompileShader(shader_id);

    GLint compiled_ok;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compiled_ok);

    GLint log_length = 0;
    glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_length);

    GLchar* log = new GLchar[log_length];
    glGetShaderInfoLog(shader_id, log_length, &log_length, log);

    if (log_length != 0)
    {
        std::string output;
        if (!compiled_ok)
        {
            output += "ERROR: OpenGL compilation of \"";
            output += filename;
            output += "\" failed.\n== Start of compilation log\n";
            output += log;
            output += "== End of compilation log\n";
        }
        else
        {
            output += "WARNING: OpenGL compilation of \"";
            output += filename;
            output += "\".\n== Start of compilation log\n";
            output += log;
            output += "== End of compilation log\n";
        }
        fprintf(stderr, "%s", output.c_str());
    }

    delete[] log;
}

GLuint ResourceManager::createGpuProgram(GLuint vertex_shader_id,
                                         GLuint fragment_shader_id)
{
    GLuint program_id = glCreateProgram();

    glAttachShader(program_id, vertex_shader_id);
    glAttachShader(program_id, fragment_shader_id);
    glLinkProgram(program_id);

    GLint linked_ok = GL_FALSE;
    glGetProgramiv(program_id, GL_LINK_STATUS, &linked_ok);

    if (linked_ok == GL_FALSE)
    {
        GLint log_length = 0;
        glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &log_length);

        GLchar* log = new GLchar[log_length];
        glGetProgramInfoLog(program_id, log_length, &log_length, log);

        std::string output;
        output += "ERROR: OpenGL linking of program failed.\n";
        output += "== Start of link log\n";
        output += log;
        output += "\n== End of link log\n";

        delete[] log;
        fprintf(stderr, "%s", output.c_str());
    }

    glDeleteShader(vertex_shader_id);
    glDeleteShader(fragment_shader_id);

    return program_id;
}

void ResourceManager::drawModel(const char* model_name)
{
    auto it = m_virtualModels.find(model_name);
    if (it == m_virtualModels.end()) {
        fprintf(stderr, "ERRO: Modelo '%s' não encontrado.\n", model_name);
        return;
    }
    const VirtualModel& model = it->second;
    glBindVertexArray(model.vertex_array_object_id);
    glUniform4f(m_bboxMinUniform, model.bbox_min.x, model.bbox_min.y, model.bbox_min.z, 1.0f);
    glUniform4f(m_bboxMaxUniform, model.bbox_max.x, model.bbox_max.y, model.bbox_max.z, 1.0f);
    glDrawElements(model.rendering_mode, (GLsizei)model.num_indices, GL_UNSIGNED_INT,
                   (void*)(model.first_index * sizeof(GLuint)));
    glBindVertexArray(0);
}