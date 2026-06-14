#include "Bezier/BezierPath.hpp"
#include <glm/glm.hpp>

class BezierMover
{
public:
    void update(float dt);

    glm::vec3 getForward() const;
    glm::vec3 getPosition() const;

private:
    BezierPath m_path;

    float m_t = 0.0f;

    float m_speed = 0.1f;
};