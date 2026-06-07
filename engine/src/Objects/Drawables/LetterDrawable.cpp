/**
 * @file    `LetterDrawable.cpp`
 * @brief   Implementação de `Letter` enquanto objeto renderizável na cena virtual.
 */
#include <cmath>
#include <glm/gtc/type_ptr.hpp>
#include "Objects/Drawables/LetterDrawable.hpp"

extern void DrawVirtualObject(const char* object_name);

/**
 * @brief Construtor padrão para `LetterDrawable`
 */
LetterDrawable::LetterDrawable() {};

/**
 * @brief   Metodo de desenho para `LetterDrawable`
 * 
 * @param   ctx (const DrawContext&)
 *          Contexto de desenho, contendo os uniformes e o ID do objeto
 *          para o shader.
 */
void LetterDrawable::draw(const DrawContext& ctx) {
    DrawVirtualObject("the_letter");
};