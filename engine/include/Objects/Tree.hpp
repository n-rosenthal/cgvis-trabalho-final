#ifndef TREE_HPP
#define TREE_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <string>

#include "Interfaces/Drawable.hpp"
#include "Interfaces/Collidable.hpp"
#include "Collision/SphereCollider.hpp"
#include "Collision/CylindricalCollider.hpp"

class Tree : public Drawable, public Collidable
{
public:
    Tree(
        const glm::vec3& position,
        const glm::vec3& rotation,
        const glm::vec3& scale,
        int type,
        const std::string& meshPath   // ← caminho do arquivo .obj
    );

    ~Tree() = default;

    // --- Drawable ---
    void buildMesh()      override;   // carrega o .obj, preenche m_vertices/m_indices
    void computeNormals() override;   // no-op se o .obj já tiver normais
    void setupBuffers()   override;   // envia para a GPU via m_buffers
    void draw(const DrawContext& ctx, int type);


    // --- Collidable ---
    std::vector<std::shared_ptr<Collider>> getColliders()   override;
    void onCollision(glm::vec3 objectPosition)              override;
    void updateColliders()                                  override;

    glm::vec3 getPosition() const { return m_position; }

private:
    void applyModelMatrix(GLint model_uniform, GLint object_id_uniform, int object_id);

private:
    glm::vec3   m_position;
    glm::vec3   m_rotation;
    glm::vec3   m_scale;
    int         m_type;
    std::string m_meshPath;

    float m_trunkHeight;
    float m_trunkRadius;
    float m_canopyRadius;

    CylindricalCollider m_trunkCollider;
    SphereCollider      m_canopyCollider;
};

#endif