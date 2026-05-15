// UIRenderer.cpp
// Implementação de UIRenderer. Veja UIRenderer.hpp para documentação.
//
// Dependências:
//   - Camera.hpp      (isPerspective, getDistance, getTheta, getPhi)
//   - InputHandler.hpp (showInfoText, showDebugPanel, getAngle*, getBunnyPosition*)
//   - textrendering.cpp (TextRendering_Init, PrintString, LineHeight, CharWidth, etc.)
//   - matrices.h: usa Matrix() em showModelViewProjection — mesma solução
//     de Camera.cpp: declarações extern em vez de incluir o header inteiro.

#include "UIRenderer.hpp"
#include "Camera.hpp"
#include "InputHandler.hpp"

#include <cstdio>   // snprintf
#include <ctime>    // time, localtime
#include <string>   // std::string — necessário antes de declarar TextRendering_PrintString

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/vec2.hpp>

// ---------------------------------------------------------------------------
// Declarações das funções de textrendering.cpp (arquivo do professor)
// ---------------------------------------------------------------------------
void TextRendering_Init();
float TextRendering_LineHeight(GLFWwindow* window);
float TextRendering_CharWidth(GLFWwindow* window);
void TextRendering_PrintString(GLFWwindow* window, const std::string& str,
                               float x, float y, float scale = 1.0f);
void TextRendering_PrintMatrixVectorProduct(GLFWwindow* window, glm::mat4 M,
                                           glm::vec4 v, float x, float y,
                                           float scale = 1.0f);
void TextRendering_PrintMatrixVectorProductMoreDigits(GLFWwindow* window,
                                                      glm::mat4 M, glm::vec4 v,
                                                      float x, float y,
                                                      float scale = 1.0f);
void TextRendering_PrintMatrixVectorProductDivW(GLFWwindow* window, glm::mat4 M,
                                               glm::vec4 v, float x, float y,
                                               float scale = 1.0f);

// ---------------------------------------------------------------------------
// matrices.h: declarar apenas Matrix() para evitar "multiple definition"
// (ver comentário em Camera.cpp)
// ---------------------------------------------------------------------------
glm::mat4 Matrix(
    float m00, float m01, float m02, float m03,
    float m10, float m11, float m12, float m13,
    float m20, float m21, float m22, float m23,
    float m30, float m31, float m32, float m33
);

// ---------------------------------------------------------------------------
// Singleton
// ---------------------------------------------------------------------------
UIRenderer& UIRenderer::get()
{
    static UIRenderer instance;
    return instance;
}

// ---------------------------------------------------------------------------
// init
// ---------------------------------------------------------------------------
void UIRenderer::init()
{
    // TextRendering_Init() requer contexto OpenGL ativo — por isso não é
    // chamada no construtor. main.cpp garante que init() vem depois de
    // glfwMakeContextCurrent().
    TextRendering_Init();
    m_initialized = true;
}

// ---------------------------------------------------------------------------
// render — ponto único de entrada para o loop de main.cpp
// ---------------------------------------------------------------------------
void UIRenderer::render(GLFWwindow* window,
                        const Camera& camera,
                        glm::mat4 view,
                        glm::mat4 projection,
                        glm::mat4 model,
                        glm::vec4 p_model)
{
    showModelViewProjection(window, projection, view, model, p_model);
    showEulerAngles(window);
    showProjection(window, camera);
    showFramesPerSecond(window);
    showDebugPanel(window, camera);
}

// ---------------------------------------------------------------------------
// showModelViewProjection
// Extraído de TextRendering_ShowModelViewProjection() em main.cpp.
// ---------------------------------------------------------------------------
void UIRenderer::showModelViewProjection(GLFWwindow* window,
                                         glm::mat4 projection,
                                         glm::mat4 view,
                                         glm::mat4 model,
                                         glm::vec4 p_model)
{
    if (!InputHandler::get().showInfoText())
        return;

    glm::vec4 p_world  = model      * p_model;
    glm::vec4 p_camera = view       * p_world;
    glm::vec4 p_clip   = projection * p_camera;
    glm::vec4 p_ndc    = p_clip / p_clip.w;

    float pad = TextRendering_LineHeight(window);

    TextRendering_PrintString(window, " Model matrix             Model     In World Coords.", -1.0f, 1.0f - pad, 1.0f);
    TextRendering_PrintMatrixVectorProduct(window, model, p_model, -1.0f, 1.0f - 2*pad, 1.0f);

    TextRendering_PrintString(window, "                                        |  ", -1.0f, 1.0f - 6*pad, 1.0f);
    TextRendering_PrintString(window, "                            .-----------'  ", -1.0f, 1.0f - 7*pad, 1.0f);
    TextRendering_PrintString(window, "                            V              ", -1.0f, 1.0f - 8*pad, 1.0f);

    TextRendering_PrintString(window, " View matrix              World     In Camera Coords.", -1.0f, 1.0f - 9*pad, 1.0f);
    TextRendering_PrintMatrixVectorProduct(window, view, p_world, -1.0f, 1.0f - 10*pad, 1.0f);

    TextRendering_PrintString(window, "                                        |  ", -1.0f, 1.0f - 14*pad, 1.0f);
    TextRendering_PrintString(window, "                            .-----------'  ", -1.0f, 1.0f - 15*pad, 1.0f);
    TextRendering_PrintString(window, "                            V              ", -1.0f, 1.0f - 16*pad, 1.0f);

    TextRendering_PrintString(window, " Projection matrix        Camera                    In NDC", -1.0f, 1.0f - 17*pad, 1.0f);
    TextRendering_PrintMatrixVectorProductDivW(window, projection, p_camera, -1.0f, 1.0f - 18*pad, 1.0f);

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    glm::vec2 a(-1.0f, -1.0f);
    glm::vec2 b(+1.0f, +1.0f);
    glm::vec2 p( 0.0f,  0.0f);
    glm::vec2 q(static_cast<float>(width), static_cast<float>(height));

    glm::mat4 viewport_mapping = Matrix(
        (q.x - p.x)/(b.x - a.x), 0.0f, 0.0f, (b.x*p.x - a.x*q.x)/(b.x - a.x),
        0.0f, (q.y - p.y)/(b.y - a.y), 0.0f, (b.y*p.y - a.y*q.y)/(b.y - a.y),
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );

    TextRendering_PrintString(window, "                                                       |  ", -1.0f, 1.0f - 22*pad, 1.0f);
    TextRendering_PrintString(window, "                            .--------------------------'  ", -1.0f, 1.0f - 23*pad, 1.0f);
    TextRendering_PrintString(window, "                            V                           ",  -1.0f, 1.0f - 24*pad, 1.0f);

    TextRendering_PrintString(window, " Viewport matrix           NDC      In Pixel Coords.", -1.0f, 1.0f - 25*pad, 1.0f);
    TextRendering_PrintMatrixVectorProductMoreDigits(window, viewport_mapping, p_ndc, -1.0f, 1.0f - 26*pad, 1.0f);
}

// ---------------------------------------------------------------------------
// showEulerAngles
// Extraído de TextRendering_ShowEulerAngles() em main.cpp.
// Lê ângulos de InputHandler em vez das globais g_AngleX/Y/Z.
// ---------------------------------------------------------------------------
void UIRenderer::showEulerAngles(GLFWwindow* window)
{
    if (!InputHandler::get().showInfoText())
        return;

    float pad = TextRendering_LineHeight(window);
    char buffer[80];
    const auto& inp = InputHandler::get();
    snprintf(buffer, 80, "Euler Angles rotation matrix = Z(%.2f)*Y(%.2f)*X(%.2f)\n",
             inp.getAngleZ(), inp.getAngleY(), inp.getAngleX());
    TextRendering_PrintString(window, buffer, -1.0f + pad/10, -1.0f + 2*pad/10, 1.0f);
}

// ---------------------------------------------------------------------------
// showProjection
// Extraído de TextRendering_ShowProjection() em main.cpp.
// Lê estado de Camera em vez da global g_UsePerspectiveProjection.
// ---------------------------------------------------------------------------
void UIRenderer::showProjection(GLFWwindow* window, const Camera& camera)
{
    if (!InputHandler::get().showInfoText())
        return;

    float lineheight = TextRendering_LineHeight(window);
    float charwidth  = TextRendering_CharWidth(window);

    const char* label = camera.isPerspective() ? "Perspective" : "Orthographic";
    TextRendering_PrintString(window, label,
                              1.0f - 13*charwidth, -1.0f + 2*lineheight/10, 1.0f);
}

// ---------------------------------------------------------------------------
// showFramesPerSecond
// Extraído de TextRendering_ShowFramesPerSecond() em main.cpp — sem mudanças
// de lógica, apenas a guarda usa InputHandler.
// ---------------------------------------------------------------------------
void UIRenderer::showFramesPerSecond(GLFWwindow* window)
{
    if (!InputHandler::get().showInfoText())
        return;

    static float old_seconds    = static_cast<float>(glfwGetTime());
    static int   elapsed_frames = 0;
    static char  buffer[20]     = "?? fps";
    static int   numchars       = 7;

    elapsed_frames += 1;

    float seconds         = static_cast<float>(glfwGetTime());
    float elapsed_seconds = seconds - old_seconds;

    if (elapsed_seconds > 1.0f)
    {
        numchars   = snprintf(buffer, 20, "%.2f fps",
                              elapsed_frames / elapsed_seconds);
        old_seconds    = seconds;
        elapsed_frames = 0;
    }

    float lineheight = TextRendering_LineHeight(window);
    float charwidth  = TextRendering_CharWidth(window);
    TextRendering_PrintString(window, buffer,
                              1.0f - (numchars + 1)*charwidth,
                              1.0f - lineheight, 1.0f);
}

// ---------------------------------------------------------------------------
// showDebugPanel
// Extraído de TextRendering_ShowDebugPanel() em main.cpp.
// Lê posição/rotação do bunny de InputHandler e dados de câmera de Camera.
// TODO: quando Game/Bird estiverem prontos, receber Game& e ler de lá.
// ---------------------------------------------------------------------------
void UIRenderer::showDebugPanel(GLFWwindow* window, const Camera& camera)
{
    if (!InputHandler::get().showDebugPanel())
        return;

    float lineheight = TextRendering_LineHeight(window);
    float charwidth  = TextRendering_CharWidth(window);

    // Hora atual
    time_t    now         = time(0);
    struct tm* currentTime = localtime(&now);
    int  hour      = currentTime->tm_hour;
    int  minute    = currentTime->tm_min;
    int  second    = currentTime->tm_sec;
    bool isDayTime = !(hour >= 18 || hour < 6);

    const auto& inp = InputHandler::get();
    char buffer[256];

    TextRendering_PrintString(window, "=== PAINEL DE VOO ===",
                              -1.0f + charwidth, 1.0f - lineheight, 1.0f);

    snprintf(buffer, 256, "Posicao: X=%.2f Y=%.2f Z=%.2f",
             inp.getBunnyPositionX(),
             inp.getBunnyPositionY(),
             inp.getBunnyPositionZ());
    TextRendering_PrintString(window, buffer,
                              -1.0f + charwidth, 1.0f - 2*lineheight, 1.0f);

    snprintf(buffer, 256, "Rotacao Y: %.2f rad (%.1f graus)",
             inp.getBunnyRotationY(),
             inp.getBunnyRotationY() * 180.0f / 3.141592f);
    TextRendering_PrintString(window, buffer,
                              -1.0f + charwidth, 1.0f - 3*lineheight, 1.0f);

    snprintf(buffer, 256, "Hora: %02d:%02d:%02d - %s",
             hour, minute, second, isDayTime ? "DIA" : "NOITE");
    TextRendering_PrintString(window, buffer,
                              -1.0f + charwidth, 1.0f - 4*lineheight, 1.0f);

    snprintf(buffer, 256, "Camera: Dist=%.2f Theta=%.2f Phi=%.2f",
             camera.getDistance(), camera.getTheta(), camera.getPhi());
    TextRendering_PrintString(window, buffer,
                              -1.0f + charwidth, 1.0f - 5*lineheight, 1.0f);

    TextRendering_PrintString(window, "Controles:",
                              -1.0f + charwidth, 1.0f - 7*lineheight, 1.0f);
    TextRendering_PrintString(window, "WASD: Mover | Q/E: Subir/Descer",
                              -1.0f + charwidth, 1.0f - 8*lineheight, 1.0f);
    TextRendering_PrintString(window, "Setas: Rotacionar | Mouse Dir: Rotacionar",
                              -1.0f + charwidth, 1.0f - 9*lineheight, 1.0f);
    TextRendering_PrintString(window, "T: Toggle Painel | H: Toggle Info",
                              -1.0f + charwidth, 1.0f - 10*lineheight, 1.0f);
}