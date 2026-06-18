/**
 * @file    `TextureLoader.cpp`
 * @brief   Loader de texturas
 */

#include "Loaders/TextureLoader.hpp"

GLuint g_NumLoadedTextures = 0;

/**
 * @brief   Função que carrega uma imagem para ser utilizada como textura
 * 
 * @param filename (const char*)
 *          caminho para o arquivo da imagem
 * @param repeat (bool, default: false)
 *          se verdadeiro, a textura será repetida ao longo dos eixos x e y
 *          senão, será clamped-to-the-edges
 */
GLuint LoadTextureImage(
    const char* filename,
    bool repeat) {
    printf("Carregando imagem \"%s\"... ", filename);

    stbi_set_flip_vertically_on_load(true);

    int width;
    int height;
    int channels;

    unsigned char* data =
        stbi_load(
            filename,
            &width,
            &height,
            &channels,
            4
        );

    if (data == NULL)
    {
        fprintf(
            stderr,
            "ERROR: Cannot open image file \"%s\".\n",
            filename
        );

        std::exit(EXIT_FAILURE);
    }

    printf("OK (%dx%d).\n", width, height);

    GLuint texture_id;
    GLuint sampler_id;

    glGenTextures(1, &texture_id);
    glGenSamplers(1, &sampler_id);

    GLint wrapMode =
        repeat ?
        GL_MIRRORED_REPEAT :
        GL_CLAMP_TO_EDGE;

    glSamplerParameteri(
        sampler_id,
        GL_TEXTURE_WRAP_S,
        wrapMode
    );

    glSamplerParameteri(
        sampler_id,
        GL_TEXTURE_WRAP_T,
        wrapMode
    );

    glSamplerParameteri(
        sampler_id,
        GL_TEXTURE_MIN_FILTER,
        GL_LINEAR_MIPMAP_LINEAR
    );

    glSamplerParameteri(
        sampler_id,
        GL_TEXTURE_MAG_FILTER,
        GL_LINEAR
    );

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
    glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);

    GLuint textureunit = g_NumLoadedTextures;

    glActiveTexture(
        GL_TEXTURE0 + textureunit
    );

    glBindTexture(
        GL_TEXTURE_2D,
        texture_id
    );

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_SRGB8_ALPHA8,
        width,
        height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        data
    );

    glGenerateMipmap(GL_TEXTURE_2D);

    glBindSampler(
        textureunit,
        sampler_id
    );

    stbi_image_free(data);

    g_NumLoadedTextures++;

    return texture_id;
}