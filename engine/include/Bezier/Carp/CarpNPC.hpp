#pragma once

#include "Objects/Interfaces/GameObject.hpp"
#include "Objects/ObjDrawable.hpp"

#include "Bezier/BezierMover.hpp"
#include "Bezier/Carp/CarpDrawable.hpp"

class CarpNPC : public GameObject {
    public:
        CarpNPC(
            glm::vec3 position,     //  posição inicial
            glm::vec3 rotation,     //  rotação inicial
            glm::vec3 scale         //  escala
        );

        //  Define curva de Bézier para animação da carpa
        void setPath(std::shared_ptr<BezierPath> path);

        //  Atualizador dinâmico para carpa
        void update(float dt);

    private:
        //  Movimentador de Bézier para carpa
        BezierMover m_mover;
};