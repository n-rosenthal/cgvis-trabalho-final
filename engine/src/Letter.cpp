#include "Letter.hpp"
#include <cmath>
#include <glm/gtc/type_ptr.hpp>

extern void DrawVirtualObject(const char* object_name);

Letter::Letter()
    : position(5.0f, -0.9f, 5.0f),
      velocity(0.0f, 0.0f, 0.0f),
      captured(false) {}

void Letter::update(float dt, const glm::vec3& bird_position, float bird_rotation, bool bird_captured) {
    captured = bird_captured;

    if (captured) {
        // Letter segue o pássaro quando capturada
        position.x = bird_position.x + claw_distance * sin(bird_rotation);
        position.z = bird_position.z + claw_distance * cos(bird_rotation);
        position.y = bird_position.y;
        velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    } else {
        // Letter não capturada: aplica gravidade
        velocity.y -= gravity * dt;
        position += velocity * dt;

        // Verificar se chegou ao chão
        if (position.y <= ground_y) {
            position.y = ground_y;
            velocity = glm::vec3(0.0f, 0.0f, 0.0f); // Sem quicar
        }
    }
}

void Letter::draw(GLuint model_uniform, GLuint object_id_uniform) const {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.25f)); // 4x menor
    glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model));
    glUniform1i(object_id_uniform, 1); // BUNNY object_id
    DrawVirtualObject("the_letter");
}
