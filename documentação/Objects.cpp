#include "Objects.h"
#include <cmath>
#include <iostream>

// Mesh
void Mesh::computeBoundingBox() {
    //  verifica `vertices` é nulo e retorna imediatamente se for o caso;
    if (vertices.empty()) return;
    
    //  define os valores iniciais de `bbox_min` e `bbox_max` como a posição do primeiro vértice do mesh (caixa delimitadora do objeto)
    bbox_min = vertices[0].position;
    bbox_max = vertices[0].position;
    
    //  itera sobre todos os vértices do mesh, atualizando `bbox_min` e `bbox_max` para encontrar os valores mínimo e máximo de cada componente (x, y, z) entre todos os vértices, resultando na caixa delimitadora do objeto
    for (const auto& v : vertices) {
        bbox_min = glm::min(bbox_min, v.position);
        bbox_max = glm::max(bbox_max, v.position);
    }
}

// SceneObject
glm::mat4 SceneObject::getModelMatrix() const {
    /*  calcula a matriz de modelagem do objeto aplicando as transformações de posição, rotação e escala na ordem correta (translação, rotação e escala) e retorna a matriz resultante, que pode ser utilizada para renderizar o objeto na cena virtual */
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, rotation.x, glm::vec3(1,0,0));
    model = glm::rotate(model, rotation.y, glm::vec3(0,1,0));
    model = glm::rotate(model, rotation.z, glm::vec3(0,0,1));
    model = glm::scale(model, scale);

    return model;
}

// Floor (chão) object
Floor::Floor(const std::string& name, float len, float wid, float y_pos)
    : SceneObject(name), length(len), width(wid) {
    /*
    Construtor para criar um objeto de chão com um nome, dimensões e posição vertical.
    Inicializa o objeto de chão com um nome, comprimento, largura e posição vertical (y_pos). 
    
    A posição vertical é definida para colocar o chão abaixo dos objetos principais da cena. 
    
    O construtor também chama a função createMesh() para gerar os vértices e índices necessários para renderizar o chão na cena virtual. 
    
    O mesh do chão é criado como um plano retangular com as dimensões especificadas, e a função createMesh() é responsável por configurar os buffers OpenGL para renderizar o chão corretamente.
    */
    position.y = y_pos;
    createMesh();
}

void Floor::createMesh() {
    /*
    Função para criar o mesh do chão, gerando os vértices e índices necessários para renderização.

    Esta função gera os vértices para um plano retangular representando o chão, com as dimensões especificadas por `length` e `width`.

    Os vértices são definidos com posições, normais (apontando para cima) e coordenadas de textura.
    */
    mesh.vertices.clear();
    
    //  Calcula as metades do comprimento e largura para definir os vértices do plano
    float half_len = length / 2.0f;
    float half_wid = width / 2.0f;
    
    // Define os vértices do plano do chão, com posição, normal e coordenadas de textura

    //  Vértice 1 (extremidade inferior esquerda)
    mesh.vertices.push_back(VertexData(
        glm::vec3(-half_len, 0, -half_wid), 
        glm::vec3(0, 1, 0), 
        glm::vec2(0, 0)
    ));

    //  Vértice 2 (extremidade inferior direita)
    mesh.vertices.push_back(VertexData(
        glm::vec3( half_len, 0, -half_wid), 
        glm::vec3(0, 1, 0), 
        glm::vec2(1, 0)
    ));

    //  Vértice 3 (extremidade superior direita)
    mesh.vertices.push_back(VertexData(
        glm::vec3( half_len, 0,  half_wid), 
        glm::vec3(0, 1, 0), 
        glm::vec2(1, 1)
    ));

    //  Vértice 4 (extremidade superior esquerda)
    mesh.vertices.push_back(VertexData(
        glm::vec3(-half_len, 0,  half_wid), 
        glm::vec3(0, 1, 0), 
        glm::vec2(0, 1)
    ));
    
    // Define os índices para formar dois triângulos que compõem o plano do chão
    mesh.indices = {0, 1, 2, 0, 2, 3};
    
    //  Calcula a caixa delimitadora do mesh do chão
    mesh.computeBoundingBox();
    
    //  Setup OpenGL buffers
    //  Gera os IDs para o Vertex Array Object (VAO), Vertex Buffer Object (VBO) e Element Buffer Object (EBO) do mesh, e configura os buffers para armazenar os dados dos vértices e índices, além de definir os atributos dos vértices para posição, normal e coordenadas de textura, preparando o mesh para renderização na cena virtual.
    glGenVertexArrays(1, &mesh.vertex_array_object_id);
    glGenBuffers(1, &mesh.vertex_buffer_id);
    glGenBuffers(1, &mesh.index_buffer_id);
    
    //  Bind VAO para configurar os buffers e atributos
    glBindVertexArray(mesh.vertex_array_object_id);
    
    //  Upload vertex data
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vertex_buffer_id);
    glBufferData(GL_ARRAY_BUFFER, 
                 mesh.vertices.size() * sizeof(VertexData),
                 mesh.vertices.data(), 
                 GL_STATIC_DRAW);
    
    // Upload index data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.index_buffer_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 mesh.indices.size() * sizeof(GLuint),
                 mesh.indices.data(),
                 GL_STATIC_DRAW);
    
    //  Define os atributos dos vértices para posição, normal e coordenadas de textura
    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, normal));
    glEnableVertexAttribArray(1);
    
    // Texcoord
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, texcoord));
    glEnableVertexAttribArray(2);
    
    //  Unbind VAO para evitar alterações acidentais
    glBindVertexArray(0);
}

void Floor::loadTexture(const char* filename) {
    /*
    Função para carregar uma textura a partir de um arquivo, configurando o ID da textura e do sampler para uso na renderização.

    Esta função utiliza a biblioteca stb_image para carregar uma imagem de textura a partir do arquivo especificado por `filename`.

    A função gera um ID de textura e um ID de sampler para o chão, configura os parâmetros de amostragem da textura (como wrap mode e filtering), e envia os dados da imagem para a GPU.
    */
    stbi_set_flip_vertically_on_load(true);
    int width, height, channels;
    unsigned char* data = stbi_load(filename, &width, &height, &channels, 3);
    
    if (!data) {
        std::cerr << "Failed to load texture: " << filename << std::endl;
        return;
    }
    
    glGenTextures(1, &texture_id);
    glGenSamplers(1, &sampler_id);
    
    glSamplerParameteri(sampler_id, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glSamplerParameteri(sampler_id, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glSamplerParameteri(sampler_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glSamplerParameteri(sampler_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8, width, height, 0, 
                 GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    stbi_image_free(data);
    has_texture = true;
}

void Floor::draw(GLuint model_uniform) const {
    /*
    Função para desenhar o chão, utilizando o shader program e enviando a matriz de modelagem como uniforme.

    Esta função é responsável por renderizar o chão na cena virtual. Ela calcula a matriz de modelagem do chão utilizando a função getModelMatrix(), envia essa matriz para o shader program através do uniforme `model_uniform`, e então desenha o mesh do chão utilizando os buffers configurados na função createMesh().
    */
    if (!visible) return;
    
    glm::mat4 model = getModelMatrix();
    glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model));
    
    if (has_texture) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_id);
        glBindSampler(0, sampler_id);
    }
    
    glBindVertexArray(mesh.vertex_array_object_id);
    glDrawElements(mesh.rendering_mode, mesh.indices.size(), 
                   GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

// Object Manager
void ObjectManager::addObject(SceneObject* obj) {
    /*
    Função para adicionar um objeto à cena virtual, armazenando o objeto em um mapa (std::unordered_map) onde a chave é o nome do objeto e o valor é um ponteiro para o objeto
    */
    objects[obj->name] = obj;
}

void ObjectManager::drawAllObjects(GLuint model_uniform) {
    /*
    Função para desenhar todos os objetos da cena virtual, utilizando o shader program e enviando a matriz de modelagem como uniforme.
    */
    for (auto& pair : objects) {
        if (pair.second->visible) {
            pair.second->draw(model_uniform);
        }
    }
}

void ObjectManager::clear() {
    /*
    Função para limpar todos os objetos da cena virtual, deletando os objetos armazenados no mapa e limpando o mapa de objetos.
    */
    for (auto& pair : objects) {
        delete pair.second;
    }
    objects.clear();
}