/**
*   Interface `Collidable`
*   Um objeto que implementa a interface `Collidable` é um objeto que
        1.  possui como um de seus campos um vetor de colisores
        2.  implementa `updateColliders()`, que atualiza os colisores
            do objeto a depender de sua posição e outros parâmetros
        3.  implementa `onCollision(glm::vec3 objectPosition)`

    isto é, um objeto é /colidível/ se ele possuir um ou mais colisores,
    que determinam o sistema de colisões ao qual participa o objeto,
    e se implementar um método que realiza as consequências de uma colisão
    contra outro objeto.
*/

#pragma once

#include <glm/glm.hpp>

#include <memory>
#include <vector>

#include "Collision/Collider.hpp"

/**
 *  Intreface `Collidable`:
 *  Necessita de um vetor de colisores e da implementação de `onCollision()`
 */
class Collidable {
    public:
        /**
        *   @brief      Retorna o vetor de colisores do objeto
        *   @returns    (std::shared_ptr<Collider>)
        */
        virtual std::vector<std::shared_ptr<Collider>> getColliders() = 0;

        /**
         *  @brief      Atualiza os colisores do objeto a depender
         *              de sua posição atual e outros parâmetros
         */
        virtual void updateColliders() = 0;

        /**
        *   @brief      Implementação das consequências de uma colisão
        *               contra outro objeto
        *
        *   @param      (glm::vec3)
        *               Posição do outro objeto
        */
        virtual void onCollision(glm::vec3 objectPosition) = 0;

        /**
        *   @brief      Destruidor padrão
        */
        virtual ~Collidable() = default;
};