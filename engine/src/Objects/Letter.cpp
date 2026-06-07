/**
 * @file    `Letter.cpp`
 * @brief   Implementação de `Letter` enquanto objeto do jogo.
 */

#include "Objects/Letter.hpp"
#include "Objects/Drawables/LetterDrawable.hpp"
#include "glad/glad.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

extern void DrawVirtualObject(const char* object_name);

/**
 * @brief   Construtor padrão para `Letter`
 * 
 * @param   position (glm::vec3)
 *          posição inicial
 * @param   rotation (glm::vec3)
 *          rotação inicial
 * @param   scale (glm::vec3)
 *          escala inicial
 */
Letter::Letter(const glm::vec3& position,
               const glm::vec3& rotation,
               const glm::vec3& scale)
    : GameObject(std::make_unique<LetterDrawable>(), position, rotation, scale) {};

/**
 * @brief   Atualizador dinâmico para `Letter`
 * 
 * @param   dt  (float)
 *          passagem do tempo
 * @param   bird_position (glm::vec3)
 *          posição atual do pássaro
 * @param   bird_rotation (glm::vec3)
 *          rotação atual do pássaro
 * @param   bird_captured (bool)
 *          verdadeiro, se o pássaro capturou a carta;
 */
void Letter::update(float dt,
                    const glm::vec3& bird_position,
                    const glm::vec3& bird_rotation,
                    bool bird_captured) {
    //  Define a flag `m_captured` com base na entrada `bird_captured`
    m_captured = bird_captured;

    //  Se a carta foi capturada, sincroniza posição e rotação com o pássaro
    if (m_captured) {
        m_position.x =  bird_position.x
                        + claw_distance
                        * sin(bird_rotation.y);
        m_position.z =  bird_position.z
                        + claw_distance
                        * cos(bird_rotation.y);
        m_position.y =  bird_position.y - 0.3f; // ligeiramente abaixo do pássaro

        m_rotation = bird_rotation;

        m_velocity = glm::vec3(0.0f); // para evitar movimento residual
    };

    //  Colisões com outros objetos
    //      ...

    //  Se a carta não foi capturada, aplica física de queda livre
    if (!m_captured) {
        m_velocity.y += gravity * dt; // aceleração gravitacional

        m_position += m_velocity * dt; // atualiza posição
    };

    //  Se cair no chão, para a carta e mantém-na no nível do terreno
    if (m_position.y <= ground_y) {
        m_position.y = ground_y;
        m_velocity = glm::vec3(0.0f);
    };
}