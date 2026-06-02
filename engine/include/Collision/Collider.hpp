#pragma once

#include <glm/glm.hpp>

/**
 * @brief   tipos de colisão
 * @details
 *  1.  =Sphere=: colisão esférica (ou esfera-esfera)
 *  2.  =AABB=: colisão retangular (bounding-boxes)
 *  3.  =Capsule=: colisão capsular
 */
enum class ColliderType {
    Sphere,
    AABB,
    Capsule,
    Cylinder
};

/**
 * @brief   Interface para colisão
 */
class Collider {
public:
    /**
     * @brief Destruidor padrão para colisores
     */
    virtual ~Collider() = default;

    /**
     * @brief   Retorna o tipo de colisão
     * @return  ColliderType
     */
    virtual ColliderType type() const = 0;
};