#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Tree.hpp"

#include "Collision/CylindricalCollider.hpp"
#include "Collision/SphereCollider.hpp"

// existe em main.cpp
extern void DrawVirtualObject(const char* object_name);

/**
 * @brief Construidor padrão para árvore
 * 
 * @param position  (glm::vec3)
 *          posição
 * @param rotation  (glm::vec3)
 *          rotação
 * @param scale     (glm::vec3)
 *          escala
 * @param type      (int)
 *          tipo de árvore (0, )
 */
Tree::Tree(
    const glm::vec3& position,
    const glm::vec3& rotation,
    const glm::vec3& scale,
    int type) : m_position(position),
                m_rotation(rotation),
                m_scale(scale),
                m_type(type),
                m_trunkHeight(4.0f * scale.y),
                m_trunkRadius(0.5f * scale.x),
                m_canopyRadius(2.0f * scale.x),

            m_trunkCollider(
                position,
                0.5f * scale.x,
                4.0f * scale.y
            ),

            m_canopyCollider(
                position + glm::vec3(
                    0.0f,
                    4.0f * scale.y,
                    0.0f
                ),
                2.0f * scale.x
            ) {};

/**
 * @brief Atualiza os colisores da árvore
 */
void Tree::updateColliders()
{
    m_trunkCollider.center = m_position;

    m_trunkCollider.radius = m_trunkRadius;

    m_trunkCollider.height = m_trunkHeight;

    m_canopyCollider.center =
        m_position +
        glm::vec3(
            0.0f,
            m_trunkHeight,
            0.0f
        );

    m_canopyCollider.radius =
        m_canopyRadius;
};

/**
 * @brief Renderizador da árvore
 * 
 * @param model_uniform 
 * @param object_id_uniform 
 * @param object_id 
 */
void Tree::draw(
    GLint model_uniform,
    GLint object_id_uniform,
    int object_id
)
{
    glm::mat4 model(1.0f);

    // =====================================
    // TRANSLAÇÃO
    // =====================================

    model = glm::translate(
        model,
        m_position
    );

    // =====================================
    // ROTAÇÕES
    // =====================================

    // yaw
    model = glm::rotate(
        model,
        m_rotation.y,
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    // pitch
    model = glm::rotate(
        model,
        m_rotation.x,
        glm::vec3(1.0f, 0.0f, 0.0f)
    );

    // roll
    model = glm::rotate(
        model,
        m_rotation.z,
        glm::vec3(0.0f, 0.0f, 1.0f)
    );

    // =====================================
    // ESCALA
    // =====================================

    model = glm::scale(
        model,
        m_scale
    );

    // =====================================
    // ENVIO PARA SHADER
    // =====================================

    glUniformMatrix4fv(
        model_uniform,
        1,
        GL_FALSE,
        glm::value_ptr(model)
    );

    glUniform1i(
        object_id_uniform,
        object_id
    );

    // =====================================
    // DESENHO
    // =====================================

    drawTreeParts(m_type);
}

void Tree::drawTreeParts(int type)
{
    if (type == 0)
    {
        DrawVirtualObject("GenTree_100_Twigs_Leaf_Bearing_Mesh");
        DrawVirtualObject("GenTree_100-Main_Trunk_Material.012_-_TRUNK");
        DrawVirtualObject("leaves.001_Material.010_-_Leaves");
        DrawVirtualObject("GenTRee_100-Branches_L2.002_Material.011_-_Secondary_Limbs");
        DrawVirtualObject("GenTRee_100-Branches_L2.002_Material");
    }
    else
    {
        DrawVirtualObject("GenTree_-_Twigs_Leaf_Bearing_tree.003");
        DrawVirtualObject("GenTree_-_Twigs_Leaf_Bearing_Material.Twigs");
        DrawVirtualObject("leaves_Material-Leaves");
        DrawVirtualObject("GenTree-Main_Trunk_Material.Trunk_and_Primary_Limbs");
    }
};