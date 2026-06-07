/**
 * @file    `LetterDrawable.hpp`
 * @brief   Implementação de `Letter` enquanto objeto renderizável na cena virtual.
 */

#ifndef LETTERDRAWABLE_HPP
#define LETTERDRAWABLE_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#pragma once



#include "Objects/Interfaces/Drawable.hpp"

class LetterDrawable : public Drawable {
    /**
     * @brief Cilindro/Letter controlado pelo sistema.
     */
    public:
        /**
         * @brief   Construtor padrão para `LetterDrawable`
         */
        LetterDrawable();

        //  Métodos necessários para toda implementação
        //  da interface `Drawable`: construção da malha
        //  de triângulos, computação dos vetores normais,
        //  setting e envio dos buffers aos shaders OpenGL
        //  são vazios pois o modelo é carregado a partir
        //  de um arquivo .obj pré-existente, e não é necessário
        //  gerar nada proceduralmente.
        void buildMesh()      override {};
        void computeNormals() override {};
        void setupBuffers()   override {};


        // Constante para tamanho padrão
        static constexpr float DEFAULT_SIZE = 0.25f;
    protected:
        //  Método de desenho específico para `LetterDrawable`
        void draw(const DrawContext& ctx) override;
};

#endif