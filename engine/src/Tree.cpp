#include "Tree.hpp"

#include <cstdlib>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Função que existe no main.cpp
extern void DrawVirtualObject(const char* object_name);

void Tree::generate(int amount)
{
    srand(10);

    trees.clear();

    for (int i = 0; i < amount; i++)
    {
        TreeInstance tree;

        tree.x = -8.0f + static_cast<float>(rand()) / RAND_MAX * 16.0f;
        tree.z = -8.0f + static_cast<float>(rand()) / RAND_MAX * 16.0f;
        tree.scale = 0.35f + static_cast<float>(rand()) / RAND_MAX * 0.35f;
        tree.rotY = static_cast<float>(rand()) / RAND_MAX * 6.28318f;

        trees.push_back(tree);
    }
}

void Tree::draw(GLint model_uniform, GLint object_id_uniform, int object_id)
{
    for (const TreeInstance& tree : trees)
    {
        glm::mat4 model = glm::mat4(1.0f);

        model = glm::translate(model, glm::vec3(tree.x, -1.1f, tree.z));
        model = glm::rotate(model, tree.rotY, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(tree.scale));

        glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model));
        glUniform1i(object_id_uniform, object_id);

        drawTreeParts(tree.type);
    }
}

void Tree::drawTreeParts(int type)
{
    /*
    DrawVirtualObject("GenTree_100_Twigs_Leaf_Bearing_Mesh");
    DrawVirtualObject("GenTree_100-Main_Trunk_Material.012_-_TRUNK");
    DrawVirtualObject("leaves.001_Material.010_-_Leaves");
    DrawVirtualObject("GenTRee_100-Branches_L2.002_Material.011_-_Secondary_Limbs");
    DrawVirtualObject("GenTRee_100-Branches_L2.002_Material");
    */
    // Usa o nome do objeto carregado em main.cpp
    DrawVirtualObject("GenTree-103_AE3D_03122023-F1");
}