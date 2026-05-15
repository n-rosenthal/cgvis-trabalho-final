#pragma once

#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

// UIRenderer.hpp
// Singleton que centraliza toda a renderização de texto/UI na janela OpenGL.
// Substitui as funções livres TextRendering_Show* que estavam em main.cpp,
// eliminando a dependência direta dessas funções nas globais g_ShowInfoText,
// g_ShowDebugPanel, g_Camera, g_BunnyPosition* etc.
//
// Funções extraídas de main.cpp:
//   TextRendering_ShowModelViewProjection() → UIRenderer::showModelViewProjection()
//   TextRendering_ShowEulerAngles()         → UIRenderer::showEulerAngles()
//   TextRendering_ShowProjection()          → UIRenderer::showProjection()
//   TextRendering_ShowFramesPerSecond()     → UIRenderer::showFramesPerSecond()
//   TextRendering_ShowDebugPanel()          → UIRenderer::showDebugPanel()
//
// TextRendering_Init() e as funções Print* permanecem em textrendering.cpp
// (arquivo do professor) — UIRenderer apenas as chama.
//
// USO em main.cpp:
//   // Inicialização (após criar a janela OpenGL):
//   UIRenderer::get().init();
//
//   // No loop de renderização, substituindo as cinco chamadas avulsas:
//   UIRenderer::get().render(window, view, projection, model, p_model);

// Forward declarations — evitam incluir Camera.hpp e InputHandler.hpp aqui,
// já que UIRenderer.cpp os inclui diretamente.
class Camera;
class InputHandler;

class UIRenderer
{
public:
    static UIRenderer& get();

    // Deve ser chamado uma vez após TextRendering_Init() (que fica em main.cpp
    // por ainda depender do contexto OpenGL já criado).
    void init();

    // Renderiza todos os elementos de UI em uma única chamada.
    // Substitui as cinco chamadas TextRendering_Show* no loop de main.cpp.
    //
    // - view/projection/model/p_model: necessários para showModelViewProjection
    //   (exibido apenas quando g_ShowInfoText está ativo via InputHandler).
    //   Passe Matrix_Identity() para model e glm::vec4(0,0,0,1) para p_model
    //   se não quiser exibir essa visualização específica.
    void render(GLFWwindow* window,
                const Camera& camera,
                glm::mat4 view,
                glm::mat4 projection,
                glm::mat4 model,
                glm::vec4 p_model);

private:
    UIRenderer() = default;
    UIRenderer(const UIRenderer&) = delete;
    UIRenderer& operator=(const UIRenderer&) = delete;

    bool m_initialized = false;

    // Equivalentes às cinco funções TextRendering_Show* de main.cpp.
    // Privadas: o loop chama apenas render(), que as coordena.
    void showModelViewProjection(GLFWwindow* window,
                                 glm::mat4 projection,
                                 glm::mat4 view,
                                 glm::mat4 model,
                                 glm::vec4 p_model);

    void showEulerAngles(GLFWwindow* window);
    void showProjection(GLFWwindow* window, const Camera& camera);
    void showFramesPerSecond(GLFWwindow* window);
    void showDebugPanel(GLFWwindow* window, const Camera& camera);
};