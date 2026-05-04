#ifndef OBJECTS_H
#define OBJECTS_H

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <unordered_map>

//  Estrutura para armazenar os dados de um vértice (posição, normal e coordenada de textura)
struct VertexData {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texcoord;
    
    // Construtor para facilitar a criação de vértices
    VertexData(glm::vec3 pos, glm::vec3 norm, glm::vec2 tex)
        : position(pos), normal(norm), texcoord(tex) {}
};

//  Estrutura para armazenar os dados de um mesh (vértices, índices, etc.)
struct Mesh {
    //  Vetor de vértices, onde cada vértice contém posição, normal e coordenada de textura
    std::vector<VertexData> vertices;

    //  Vetor de índices para renderização com glDrawElements
    std::vector<GLuint> indices;

    //  Modo de renderização (GL_TRIANGLES, GL_TRIANGLE_STRIP, etc.)
    GLenum rendering_mode = GL_TRIANGLES;

    //  bbox: Axis-Aligned Bounding Box do mesh, utilizado para colisões e culling
    glm::vec3 bbox_min;
    glm::vec3 bbox_max;

    //  vertex_array_object_id: ID do VAO onde estão armazenados os atributos do mesh
    GLuint vertex_array_object_id = 0;

    //  vertex_buffer_id: ID do VBO onde estão armazenados os dados dos vértices
    GLuint vertex_buffer_id = 0;

    //  index_buffer_id: ID do EBO onde estão armazenados os índices para glDrawElements
    GLuint index_buffer_id = 0;
    

    //  Função para calcular a Axis-Aligned Bounding Box (AABB) do mesh, atualizando bbox_min e bbox_max
    void computeBoundingBox();
};

//  Classe-base para objetos da cena virtual
class SceneObject {
public:
    //  Nome do objeto, utilizado para identificação e acesso
    std::string name;

    //  Transformações básicas do objeto: posição, rotação (Euler angles) e escala
    glm::vec3 position;
    glm::vec3 rotation; // Euler angles
    glm::vec3 scale;

    //  Cor do objeto, utilizada para renderização (pode ser ignorada se o objeto tiver textura)
    glm::vec4 color;

    //  Visibilidade do objeto, controlada pelo usuário para mostrar ou esconder o objeto na cena
    bool visible;
    
    //  Construtor para inicializar o objeto com um nome e valores padrão para transformações, cor e visibilidade
    SceneObject(const std::string& n) 
        : name(n), position(0.0f), rotation(0.0f), scale(1.0f), 
          color(1.0f), visible(true) {}
    
    //  Destrutor virtual para permitir a herança correta
    virtual ~SceneObject() {}

    //  Função virtual para obter a matriz de modelagem do objeto, combinando posição, rotação e escala
    virtual glm::mat4 getModelMatrix() const;

    //  Função virtual para desenhar o objeto, utilizando o shader program e enviando a matriz de modelagem como uniforme
    virtual void draw(GLuint model_uniform) const = 0;
};

// Specific floor object
class Floor : public SceneObject {
public:
    float length;  // X-axis
    float width;   // Z-axis
    
    //  Construtor para criar um objeto de chão com um nome, dimensões e posição vertical
    Floor(const std::string& name, float len, float wid, float y_pos = -1.1f);
    
    //  Função para obter a matriz de modelagem do chão, aplicando a translação vertical e escala para as dimensões do chão
    virtual glm::mat4 getModelMatrix() const override;

    //  Função para desenhar o chão, utilizando o shader program e enviando a matriz de modelagem como uniforme
    virtual void draw(GLuint model_uniform) const override;
    
    //  Função para criar o mesh do chão, gerando os vértices e índices necessários para renderização
    void createMesh();

    //  Função para carregar uma textura a partir de um arquivo, configurando o ID da textura e do sampler para uso na renderização
    void loadTexture(const char* filename);
    
private:
    //  Mesh do chão
    Mesh mesh;

    //  IDs de textura e sampler para o chão, utilizados para renderização com textura
    GLuint texture_id = 0;
    GLuint sampler_id = 0;

    //  flag booleana para indicar se o chão tem uma textura carregada, controlando a renderização com ou sem textura
    bool has_texture = false;
};

// Sphere object
class Sphere : public SceneObject {
public:
    float radius;
    int sectors;
    int stacks;
    
    Sphere(const std::string& name, float r = 0.5f, int sec = 36, int stk = 18);
    virtual void draw(GLuint model_uniform) const override;
    void createMesh();
    
private:
    Mesh mesh;
};

// Bunny object
class Bunny : public SceneObject {
public:
    Bunny(const std::string& name, const char* filename);
    virtual void draw(GLuint model_uniform) const override;
    void loadFromFile(const char* filename);
    
private:
    Mesh mesh;
};

class ObjectManager {
    /*
    Classe para gerenciar os objetos da cena virtual, permitindo adicionar, remover e acessar objetos por nome, além de desenhar todos os objetos na cena. 
    Utiliza um std::unordered_map para armazenar os objetos, onde a chave é o nome do objeto e o valor é um ponteiro para o objeto. 
    Fornece métodos para adicionar um objeto, remover um objeto por nome, obter um objeto por nome, desenhar todos os objetos utilizando um shader program e limpar todos os objetos da cena.
    Implementa o padrão singleton para garantir que haja apenas uma instância do ObjectManager em todo o programa.
    */
public:
    //  Função para obter a instância única do ObjectManager, garantindo que haja apenas um gerenciador de objetos em todo o programa
    static ObjectManager& getInstance() {
        static ObjectManager instance;
        return instance;
    }
    
    //  Função para adicionar um objeto à cena, armazenando-o no mapa de objetos com seu nome como chave
    void addObject(SceneObject* obj);

    //  Função para remover um objeto da cena por nome, verificando se o objeto existe antes de removê-lo do mapa e liberar a memória alocada para o objeto
    void removeObject(const std::string& name);

    //  Função para obter um objeto da cena por nome, retornando um ponteiro para o objeto se ele existir no mapa ou nullptr caso contrário
    SceneObject* getObject(const std::string& name);

    //  Função para desenhar todos os objetos da cena, iterando sobre o mapa de objetos e chamando a função draw de cada objeto visível, passando o ID do uniforme de modelo para cada objeto
    void drawAllObjects(GLuint model_uniform);

    //  Função para limpar todos os objetos da cena, removendo todos os objetos do mapa e liberando a memória alocada para cada objeto
    void clear();
    
private:
    // Construtor privado para implementar o padrão singleton, evitando que outras partes do código possam criar instâncias adicionais do ObjectManager
    ObjectManager() {}

    //  Mapa de objetos da cena, onde a chave é o nome do objeto e o valor é um ponteiro para o objeto, permitindo acesso rápido por nome e gerenciamento eficiente dos objetos da cena virtual
    std::unordered_map<std::string, SceneObject*> objects;
};

#endif