/**
 * @file    `Camera.cpp`
 * @brief   Implementação de câmeras ao jogo
 */

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "Game/Camera.hpp"
#include "matrices.h"

/**
 * @brief Construtor padrão para câmera
 */
Camera::Camera() {
    //  matriz de posição e de alvo para câmera
    m_position = glm::vec3(0);
    m_target   = glm::vec3(0);
};


/**
 * @brief Atualizador dinâmico da câmera
 * 
 * @param targetPos     (glm::vec3)
 *          nova posição do alvo
 * @param targetForward (glm::vec3)
 *          novo forward (vetor direção) do alvo
 * @param targetUp      (glm::vec3)
 *          novo up (vetor vertical) do alvo
 * @param dt            (float)
 *          passagem do tempo
 */
void Camera::update(
    const glm::vec3& targetPos,
    const glm::vec3& targetForward,
    const glm::vec3& targetUp,
    float dt) {
    
        
    glm::vec3 worldUp(0,1,0);

    m_position =
        targetPos
        - targetForward * 8.0f
        + worldUp * 3.0f;

    m_target =
        targetPos
        + targetForward * 4.0f;
    };

/**
 * @brief   Posiciona a c\u00e2mera num ponto de vista fixo relativo a `target`.
 *          Usado para modos de c\u00e2mera especiais (carta em voo, tela de vit\u00f3ria).
 *
 * @param target     ponto de interesse (a c\u00e2mera olha para c\u00e1)
 * @param armOffset  deslocamento da c\u00e2mera relativo ao alvo (ex: {0, 6, 12})
 */
void Camera::lookAt(const glm::vec3& target, const glm::vec3& armOffset)
{
    m_position = target + armOffset;
    m_target   = target;
};

/**
 * @brief Acessador à matriz view da câmera
 * 
 * @return glm::mat4
 *          matriz view da câmera
 */
glm::mat4 Camera::getViewMatrix() const {
    glm::vec4 pos = glm::vec4(m_position,1);
    glm::vec4 look = glm::vec4(m_target,1);
    glm::vec4 up = glm::vec4(0,1,0,0);


    // // depuração
    // glm::vec4 viewDir = look - pos;
    // printf(
    //     "VIEW DIR = %.2f %.2f %.2f\n",
    //     viewDir.x,
    //     viewDir.y,
    //     viewDir.z
    // );

    glm::vec4 u = look - pos;
    float u_norm = norm(u);

    if (u_norm < 0.0001f) {
        printf("CAMERA DEGENERATED!\n");

        u = glm::vec4(1,0,0,0);
    } else {
        u /= u_norm;
    }

    return Matrix_Camera_View(
        pos,
        look-pos,
        up
    );
}