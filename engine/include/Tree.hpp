#ifndef TREE_HPP
#define TREE_HPP

#include <vector>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

struct TreeInstance
{
    float x;
    float z;
    float scale;
    float rotY;
};

class Tree
{
public:
    void generate(int amount);
    void draw(GLint model_uniform, GLint object_id_uniform, int object_id);

private:
    std::vector<TreeInstance> trees;

    void drawTreeParts();
};

#endif