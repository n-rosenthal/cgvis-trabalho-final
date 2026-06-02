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
    int type,
    const std::string& meshPath)
        : 
            //  matriz de posição
            m_position(position),

            //  matriz de rotação
            m_rotation(rotation),

            //  matriz de escala
            m_scale(scale),

            //  tipo da árvore
            m_type(type),

            //  altura, raio do tronco
            m_trunkHeight(4.0f * scale.y),
            m_trunkRadius(0.5f * scale.x),

            //  raio dos galhos, folhas
            m_canopyRadius(2.0f * scale.x),

            //  colisor cilíndrico para o tronco
            m_trunkCollider(
                position,
                0.5f * scale.x,
                4.0f * scale.y
            ),

            //  colisor esférico para a copa
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
void Tree::updateColliders() {
    //  centro do colisor do tronco <- posição da árvore
    m_trunkCollider.center = m_position;

    //  raio, altura do colisor do tronco <- raio, altura do tronco
    m_trunkCollider.radius = m_trunkRadius;
    m_trunkCollider.height = m_trunkHeight;

    //  centro, raio do colisor da copa <- centro, raio da copa
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


void Tree::draw(const DrawContext& ctx, int type){
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