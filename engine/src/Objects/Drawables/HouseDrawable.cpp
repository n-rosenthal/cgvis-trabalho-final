#include "Objects/Drawables/HouseDrawable.hpp"

HouseDrawable::HouseDrawable()
{
    generate();
}

void HouseDrawable::buildMesh()
{
    m_vertices.clear();
    m_indices.clear();

    auto addVertex =
        [&](float x,float y,float z,
            float r,float g,float b)
    {
        Vertex v;

        v.position = {x,y,z};
        v.normal   = {0,1,0};
        v.texcoord = {0,0};
        v.color    = {r,g,b};

        m_vertices.push_back(v);
    };

    // Corpo da casa
    addVertex(-1,0,-1, 0.8f,0.7f,0.5f);
    addVertex( 1,0,-1, 0.8f,0.7f,0.5f);
    addVertex( 1,0, 1, 0.8f,0.7f,0.5f);
    addVertex(-1,0, 1, 0.8f,0.7f,0.5f);

    addVertex(-1,2,-1, 0.8f,0.7f,0.5f);
    addVertex( 1,2,-1, 0.8f,0.7f,0.5f);
    addVertex( 1,2, 1, 0.8f,0.7f,0.5f);
    addVertex(-1,2, 1, 0.8f,0.7f,0.5f);

    // Cumeeira
    addVertex(0,3,-1, 0.7f,0.1f,0.1f);
    addVertex(0,3, 1, 0.7f,0.1f,0.1f);

    m_indices =
    {
        // frente
        0,1,5,
        0,5,4,

        // direita
        1,2,6,
        1,6,5,

        // trás
        2,3,7,
        2,7,6,

        // esquerda
        3,0,4,
        3,4,7,

        // telhado
        4,7,9,
        4,9,8,

        5,8,9,
        5,9,6,

        4,8,5,
        7,6,9
    };
}

void HouseDrawable::computeNormals()
{
    for(auto& v : m_vertices)
        v.normal = glm::vec3(0,1,0);
}

void HouseDrawable::setupBuffers()
{
    glGenVertexArrays(1,&m_buffers.VAO);
    glGenBuffers(1,&m_buffers.VBO);
    glGenBuffers(1,&m_buffers.EBO);
    glBindVertexArray(m_buffers.VAO);

    glBindBuffer(GL_ARRAY_BUFFER,m_buffers.VBO);
    glBufferData(GL_ARRAY_BUFFER,(GLsizeiptr)(m_vertices.size()*sizeof(Vertex)),
                 m_vertices.data(),GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,m_buffers.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,(GLsizeiptr)(m_indices.size()*sizeof(GLuint)),
                 m_indices.data(),GL_STATIC_DRAW);

    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex,position));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex,normal));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex,texcoord));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex,color));
    glEnableVertexAttribArray(3);

    glBindVertexArray(0);
}

void HouseDrawable::draw(
    const DrawContext&
)
{
    glBindVertexArray(m_buffers.VAO);

    glDrawElements(
        GL_TRIANGLES,
        static_cast<GLsizei>(m_indices.size()),
        GL_UNSIGNED_INT,
        nullptr
    );

    glBindVertexArray(0);
}