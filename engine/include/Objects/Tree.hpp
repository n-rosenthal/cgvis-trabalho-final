#ifndef TREE_HPP
#define TREE_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Collision/SphereCollider.hpp"
#include "Collision/CylindricalCollider.hpp"

class Tree
{
public:

    Tree(
        const glm::vec3& position,
        const glm::vec3& rotation,
        const glm::vec3& scale,
        int type
    );

    ~Tree() = default;

    void draw(
        GLint model_uniform,
        GLint object_id_uniform,
        int object_id
    );

    void updateColliders();

    const CylindricalCollider& getTrunkCollider() const
    {
        return m_trunkCollider;
    }

    const SphereCollider& getCanopyCollider() const
    {
        return m_canopyCollider;
    }

    glm::vec3 getPosition() const
    {
        return m_position;
    }

private:

    void drawTreeParts(int type);

private:

    glm::vec3 m_position;
    glm::vec3 m_rotation;
    glm::vec3 m_scale;

    int m_type;

    float m_trunkHeight;
    float m_trunkRadius;
    float m_canopyRadius;

    CylindricalCollider m_trunkCollider;
    SphereCollider m_canopyCollider;
};

#endif