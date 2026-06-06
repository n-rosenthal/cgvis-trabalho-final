/**
 * @file    RockDrawable.hpp
 * @brief   Drawable para ProceduralRock.
 *          Encapsula a geometria procedural e os buffers OpenGL
 *          da rocha, delegando a lógica de mesh à própria classe.
 */
#pragma once

#include "Objects/Interfaces/Drawable.hpp"

class RockDrawable : public Drawable {
public:
    explicit RockDrawable(int subdivisions);

    void buildMesh()      override;
    void computeNormals() override;
    void setupBuffers()   override;

protected:
    void draw(const DrawContext& ctx) override;

private:
    int m_subdivisions;

    // Vértice interno — layout idêntico ao RockVertex original
    struct RockVertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texcoords;
    };

    std::vector<RockVertex> m_rockVerts;
    std::vector<GLuint>     m_rockIdx;

    static float noise(float x, float y, float z);
    static float fbm(glm::vec3 p);
};