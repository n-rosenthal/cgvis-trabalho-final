#ifndef OBJECT_FACTORY_H
#define OBJECT_FACTORY_H

#include "Objects.h"

class ObjectFactory {
    /*
    Classe-fábrica para criação de objetos da cena virtual, como chão, esferas, etc. Esta classe contém métodos estáticos para criar objetos pré-definidos com propriedades padrão, facilitando a criação e configuração de objetos na cena virtual.
    
    Os métodos de criação de objetos (como createFloor, createCheckeredFloor e createSphere) retornam ponteiros para os objetos criados, que podem ser adicionados à cena virtual utilizando
    */
public:
    static Floor* createFloor(float length, float width, float y_pos = -1.1f) {
        /*
        Função para criar um objeto de chão com um nome, dimensões e posição vertical.
        */
        Floor* floor = new Floor("floor_" + std::to_string(rand()), length, width, y_pos);
        
        // Default floor properties
        floor->color = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
        
        return floor;
    }
    
    static Floor* createCheckeredFloor(float length, float width, float y_pos = -1.1f) {
        /*
        Função para criar um objeto de chão xadrezado com um nome, dimensões e posição vertical.
        */
        Floor* floor = createFloor(length, width, y_pos);

        //  note que é esperado um diretório "textures" contendo a imagem "checkered.png" para que esta função funcione corretamente
        floor->loadTexture("textures/checkered.png");
        return floor;
    }
    
    static Sphere* createSphere(float radius, glm::vec3 position) {
        /*
        Função para criar um objeto de esfera com um nome, raio e posição.
        */
        Sphere* sphere = new Sphere("sphere_" + std::to_string(rand()), radius);
        sphere->position = position;
        sphere->color = glm::vec4(0.8f, 0.2f, 0.2f, 1.0f);
        return sphere;
    }
};

#endif // OBJECT_FACTORY_H