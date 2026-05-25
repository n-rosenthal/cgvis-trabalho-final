/** Header para implementação da Letter (cilindro)
*/

#ifndef LETTER_HPP
#define LETTER_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Letter {
    /** Cilindro/letter controlado pelo sistema */
public:
    // Construtor
    Letter();

    // Atualiza o estado da letter
    void update(float dt, const glm::vec3& bird_position, float bird_rotation, bool bird_captured);

    // Desenha a letter
    void draw(GLuint model_uniform, GLuint object_id_uniform) const;

    // Getters
    glm::vec3 getPosition() const { return position; }
    bool isCaptured() const { return captured; }

    // Setters
    void setCaptured(bool captured) { this->captured = captured; }
    void setPosition(const glm::vec3& pos) { position = pos; }

    // Tamanho da carta
    glm::vec3 getSize() const { return size; }
    void setSize(const glm::vec3& s) { size = s; }

private:
    glm::vec3 position;      // X, Y, Z
    glm::vec3 velocity;      // Velocidade da letter
    glm::vec3 size;          // Tamanho da carta (escala)
    bool captured;           // Flag indicando se foi capturada pelo pássaro

    // Parâmetros de física
    const float gravity = 9.8f;
    const float ground_y = -1.0f;  // Altura do chão
    const float claw_distance = 0.5f;  // Distância das garras do pássaro
};

#endif
