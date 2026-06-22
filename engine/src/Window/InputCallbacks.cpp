#include "textrendering.hpp"

#include "Game/Window.hpp" //   variáveis de depuração do pássaro no menu de debug

#include "Window/InputCallbacks.hpp"
#include "Window/WindowCallbacks.hpp"
#include "Renderer/ShaderLoader.hpp"

bool     g_ShowInfoText;
bool     g_ShowDebugPanel;
bool     g_UsePerspectiveProjection;
bool     g_ManualDayNight;
bool     g_DayTime;
float    g_AngleX;
float    g_AngleY;
float    g_AngleZ;

double    g_LastCursorPosX;
double    g_LastCursorPosY;
bool     g_LeftMouseButtonPressed;
bool     g_RightMouseButtonPressed;
bool     g_MiddleMouseButtonPressed;

float g_CameraTheta;
float g_CameraPhi;
float g_CameraDistance;

// Definição da função que será chamada sempre que o usuário pressionar alguma
// tecla do teclado. Veja http://www.glfw.org/docs/latest/input_guide.html#input_key
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mod)
{
    // =======================
    // Não modifique esta chamada! Ela é utilizada para correção automatizada dos
    // laboratórios. Deve ser sempre o primeiro comando desta função KeyCallback().
    Correcao_KeyCallback(key, action, mod);
    // =======================

    // Se o usuário pressionar a tecla ESC, fechamos a janela.
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
        fprintf(stdout,"Janela fechada.\n");

    }
    // O código abaixo implementa a seguinte lógica:
    //   Se apertar tecla X       então g_AngleX += delta;
    //   Se apertar tecla shift+X então g_AngleX -= delta;
    //   Se apertar tecla Y       então g_AngleY += delta;
    //   Se apertar tecla shift+Y então g_AngleY -= delta;
    //   Se apertar tecla Z       então g_AngleZ += delta;
    //   Se apertar tecla shift+Z então g_AngleZ -= delta;

    float delta = 3.141592 / 16; // 22.5 graus, em radianos.

    if (key == GLFW_KEY_X && action == GLFW_PRESS)
    {
        g_AngleX += (mod & GLFW_MOD_SHIFT) ? -delta : delta;
    }

    if (key == GLFW_KEY_Y && action == GLFW_PRESS)
    {
        g_AngleY += (mod & GLFW_MOD_SHIFT) ? -delta : delta;
    }
    if (key == GLFW_KEY_Z && action == GLFW_PRESS)
    {
        g_AngleZ += (mod & GLFW_MOD_SHIFT) ? -delta : delta;
    }

    // Se o usuário apertar a tecla espaço, resetamos os ângulos de Euler para zero.
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
    {
        g_AngleX = 0.0f;
        g_AngleY = 0.0f;
        g_AngleZ = 0.0f;
    }

    // Se o usuário apertar a tecla P, utilizamos projeção perspectiva.
    if (key == GLFW_KEY_P && action == GLFW_PRESS)
    {
        g_UsePerspectiveProjection = true;
        fprintf(stdout,"Alteracao de perspectiva para perspectiva.\n");

    }

    // Se o usuário apertar a tecla O, utilizamos projeção ortográfica.
    if (key == GLFW_KEY_O && action == GLFW_PRESS)
    {
        g_UsePerspectiveProjection = false;
        fprintf(stdout,"Alteracao de perspectiva para ortografica.\n");

    }

    // Se o usuário apertar a tecla H, fazemos um "toggle" do texto informativo mostrado na tela.
    if (key == GLFW_KEY_H && action == GLFW_PRESS)
    {
        g_ShowInfoText = !g_ShowInfoText;
        fprintf(stdout,"Texto informativo toggled.\n");

    }

    // Se o usuario apertar a tecla T, fazemos um "toggle" do painel de depuracao.
    if (key == GLFW_KEY_M && action == GLFW_PRESS)
    {
        g_ShowDebugPanel = !g_ShowDebugPanel;
        fprintf(stdout,"Painel de depuracao toggled.\n");

    }

    // // Se o usuário apertar a tecla L, alternamos dia/noite manualmente além da lógica automática.
    // if (key == GLFW_KEY_L && action == GLFW_PRESS)
    // {
    //     g_ManualDayNight = true;
    //     g_DayTime = !g_DayTime;
    //     fprintf(stdout,"Iluminacao do mundo trocada.\n");
    // }

    // Se o usuário apertar a tecla R, recarregamos os shaders dos arquivos "shader_fragment.glsl" e "shader_vertex.glsl".
    if (key == GLFW_KEY_R && action == GLFW_PRESS)
    {
        LoadShadersFromFiles();
        fprintf(stdout,"Shaders recarregados!\n");
        fflush(stdout);
    }

    // Controles de voo do Bird são tratados pela classe Bird::update().
    // Esta função já responde a WASD + Q/E para ajustar velocidade, rotação e altura.
}



// Esta função recebe um vértice com coordenadas de modelo p_model e passa o
// mesmo por todos os sistemas de coordenadas armazenados nas matrizes model,
// view, e projection; e escreve na tela as matrizes e pontos resultantes
// dessas transformações.
void TextRendering_ShowModelViewProjection(
    GLFWwindow* window,
    glm::mat4 projection,
    glm::mat4 view,
    glm::mat4 model,
    glm::vec4 p_model
)
{
    if ( !g_ShowInfoText )
        return;

    glm::vec4 p_world = model*p_model;
    glm::vec4 p_camera = view*p_world;
    glm::vec4 p_clip = projection*p_camera;
    glm::vec4 p_ndc = p_clip / p_clip.w;

    float pad = TextRendering_LineHeight(window);

    TextRendering_PrintString(window, " Model matrix             Model     In World Coords.", -1.0f, 1.0f-pad, 1.0f);
    TextRendering_PrintMatrixVectorProduct(window, model, p_model, -1.0f, 1.0f-2*pad, 1.0f);

    TextRendering_PrintString(window, "                                        |  ", -1.0f, 1.0f-6*pad, 1.0f);
    TextRendering_PrintString(window, "                            .-----------'  ", -1.0f, 1.0f-7*pad, 1.0f);
    TextRendering_PrintString(window, "                            V              ", -1.0f, 1.0f-8*pad, 1.0f);

    TextRendering_PrintString(window, " View matrix              World     In Camera Coords.", -1.0f, 1.0f-9*pad, 1.0f);
    TextRendering_PrintMatrixVectorProduct(window, view, p_world, -1.0f, 1.0f-10*pad, 1.0f);

    TextRendering_PrintString(window, "                                        |  ", -1.0f, 1.0f-14*pad, 1.0f);
    TextRendering_PrintString(window, "                            .-----------'  ", -1.0f, 1.0f-15*pad, 1.0f);
    TextRendering_PrintString(window, "                            V              ", -1.0f, 1.0f-16*pad, 1.0f);

    TextRendering_PrintString(window, " Projection matrix        Camera                    In NDC", -1.0f, 1.0f-17*pad, 1.0f);
    TextRendering_PrintMatrixVectorProductDivW(window, projection, p_camera, -1.0f, 1.0f-18*pad, 1.0f);

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    glm::vec2 a = glm::vec2(-1, -1);
    glm::vec2 b = glm::vec2(+1, +1);
    glm::vec2 p = glm::vec2( 0,  0);
    glm::vec2 q = glm::vec2(width, height);

    glm::mat4 viewport_mapping = Matrix(
        (q.x - p.x)/(b.x-a.x), 0.0f, 0.0f, (b.x*p.x - a.x*q.x)/(b.x-a.x),
        0.0f, (q.y - p.y)/(b.y-a.y), 0.0f, (b.y*p.y - a.y*q.y)/(b.y-a.y),
        0.0f , 0.0f , 1.0f , 0.0f ,
        0.0f , 0.0f , 0.0f , 1.0f
    );

    TextRendering_PrintString(window, "                                                       |  ", -1.0f, 1.0f-22*pad, 1.0f);
    TextRendering_PrintString(window, "                            .--------------------------'  ", -1.0f, 1.0f-23*pad, 1.0f);
    TextRendering_PrintString(window, "                            V                           ", -1.0f, 1.0f-24*pad, 1.0f);

    TextRendering_PrintString(window, " Viewport matrix           NDC      In Pixel Coords.", -1.0f, 1.0f-25*pad, 1.0f);
    TextRendering_PrintMatrixVectorProductMoreDigits(window, viewport_mapping, p_ndc, -1.0f, 1.0f-26*pad, 1.0f);
}

// Escrevemos na tela os ângulos de Euler definidos nas variáveis globais
// g_AngleX, g_AngleY, e g_AngleZ.
void TextRendering_ShowEulerAngles(GLFWwindow* window)
{
    if ( !g_ShowInfoText )
        return;

    float pad = TextRendering_LineHeight(window);

    char buffer[80];
    snprintf(buffer, 80, "Euler Angles: Z=%.2f Y=%.2f X=%.2f", g_AngleZ, g_AngleY, g_AngleX);

    TextRendering_PrintString(window, buffer, -1.0f + pad/10, -1.0f + 2*pad/10, 1.0f);
}

// Escrevemos na tela qual matriz de projeção está sendo utilizada.
void TextRendering_ShowProjection(GLFWwindow* window)
{
    if ( !g_ShowInfoText )
        return;

    float lineheight = TextRendering_LineHeight(window);
    float charwidth = TextRendering_CharWidth(window);

    if ( g_UsePerspectiveProjection )
        TextRendering_PrintString(window, "Projecao: Perspectiva", 1.0f-22*charwidth, -1.0f+2*lineheight/10, 1.0f);
    else
        TextRendering_PrintString(window, "Projecao: Ortografica", 1.0f-23*charwidth, -1.0f+2*lineheight/10, 1.0f);
}

// Escrevemos na tela o número de quadros renderizados por segundo (frames per
// second).
void TextRendering_ShowFramesPerSecond(GLFWwindow* window)
{
    if ( !g_ShowInfoText )
        return;

    // Variáveis estáticas (static) mantém seus valores entre chamadas
    // subsequentes da função!
    static float old_seconds = (float)glfwGetTime();
    static int   ellapsed_frames = 0;
    static char  buffer[20] = "?? fps";
    static int   numchars = 7;

    ellapsed_frames += 1;

    // Recuperamos o número de segundos que passou desde a execução do programa
    float seconds = (float)glfwGetTime();

    // Número de segundos desde o último cálculo do fps
    float ellapsed_seconds = seconds - old_seconds;

    if ( ellapsed_seconds > 1.0f )
    {
        numchars = snprintf(buffer, 20, "%.2f fps", ellapsed_frames / ellapsed_seconds);
    
        old_seconds = seconds;
        ellapsed_frames = 0;
    }

    float lineheight = TextRendering_LineHeight(window);
    float charwidth = TextRendering_CharWidth(window);

    TextRendering_PrintString(window, buffer, 1.0f-(numchars + 1)*charwidth, 1.0f-lineheight, 1.0f);
}

void TextRendering_ShowDebugPanel(GLFWwindow* window)
{
    if (!g_ShowDebugPanel)
    {
        float lineheight = TextRendering_LineHeight(window);
        float charwidth = TextRendering_CharWidth(window);

        TextRendering_PrintString(
            window,
            "Pressione M para mostrar o painel",
            -1.0f + charwidth,
            1.0f - lineheight,
            1.0f
        );

        return;
    }

    float lineheight = TextRendering_LineHeight(window);
    float charwidth = TextRendering_CharWidth(window);

    // Obtém hora atual
    time_t now = time(0);
    struct tm* currentTime = localtime(&now);
    int hour = currentTime->tm_hour;
    int minute = currentTime->tm_min;
    int second = currentTime->tm_sec;
    bool actualDayTime = !(hour >= 18 || hour < 6);
    bool isDayTime = g_ManualDayNight ? g_DayTime : actualDayTime;
    const char* timeMode = g_ManualDayNight ? "Manual" : "Automatico";

    // Título do painel
    TextRendering_PrintString(window, "=== PAINEL DE INFORMAÇÕES ===", -1.0f + charwidth, 1.0f - lineheight, 1.0f);

    // Informações de data e hora
    TextRendering_PrintString(window, "Data e hora:", -1.0f + charwidth, 1.0f - 2*lineheight, 1.0f);
    TextRendering_PrintString(window, asctime(currentTime), -1.0f + charwidth, 1.0f - 3*lineheight, 1.0f);


    //  Informações do Pássaro
    //  Posição
    char birdPosition_buffer[128];

    snprintf(
        birdPosition_buffer,
        sizeof(birdPosition_buffer),
        "Bird Pos: (%.1f %.1f %.1f)",
        g_DebugBirdPosition.x,
        g_DebugBirdPosition.y,
        g_DebugBirdPosition.z
    );

    TextRendering_PrintString(
        window,
        birdPosition_buffer,
        -1.0f + charwidth,
        1.0f - 5*lineheight,
        1.0f
    );

    //  Velocidade
    char birdVelocity_buffer[128];

    snprintf(
        birdVelocity_buffer,
        sizeof(birdVelocity_buffer),
        "Bird Vel: (%.1f %.1f %.1f)",
        g_DebugBirdVelocity.x,
        g_DebugBirdVelocity.y,
        g_DebugBirdVelocity.z
    );

    TextRendering_PrintString(
        window,
        birdVelocity_buffer,
        -1.0f + charwidth,
        1.0f - 6*lineheight,
        1.0f
    );

    //  Velocidade (magnitude)
    char birdSpeed_buffer[128];

    snprintf(
        birdSpeed_buffer,
        sizeof(birdSpeed_buffer),
        "Bird Speed: %.3f",
        glm::length(g_DebugBirdVelocity)
    );

    TextRendering_PrintString(
        window,
        birdSpeed_buffer,
        -1.0f + charwidth,
        1.0f - 7*lineheight,
        1.0f
    );

    //  Rotação
    char birdRotation_buffer[128];

    snprintf(
        birdRotation_buffer,
        sizeof(birdRotation_buffer),
        "Bird Rot: (%.1f %.1f %.1f)",
        g_DebugBirdRotation.x,
        g_DebugBirdRotation.y,
        g_DebugBirdRotation.z
    );

    TextRendering_PrintString(
        window,
        birdRotation_buffer,
        -1.0f + charwidth,
        1.0f - 8*lineheight,
        1.0f
    );

    // Informações sobre a carta (letter)
    //  Posição
    char letterPosition_buffer[128];

    snprintf(
        letterPosition_buffer,
        sizeof(letterPosition_buffer),
        "Letter Pos: (%.1f %.1f %.1f)",
        g_DebugLetterPosition.x,
        g_DebugLetterPosition.y,
        g_DebugLetterPosition.z
    );

    TextRendering_PrintString(
        window,
        letterPosition_buffer,
        -1.0f + charwidth,
        1.0f - 10*lineheight,
        1.0f
    );

    //  Velocidade
    char letterVelocity_buffer[128];

    snprintf(
        letterVelocity_buffer,
        sizeof(letterVelocity_buffer),
        "Letter Vel: (%.1f %.1f %.1f)",
        g_DebugLetterVelocity.x,
        g_DebugLetterVelocity.y,
        g_DebugLetterVelocity.z
    );

    TextRendering_PrintString(
        window,
        letterVelocity_buffer,
        -1.0f + charwidth,
        1.0f - 11*lineheight,
        1.0f
    );

    //  Velocidade (magnitude)
    char letterSpeed_buffer[128];

    snprintf(
        letterSpeed_buffer,
        sizeof(letterSpeed_buffer),
        "Letter Speed: %.3f",
        glm::length(g_DebugLetterVelocity)
    );

    TextRendering_PrintString(
        window,
        letterSpeed_buffer,
        -1.0f + charwidth,
        1.0f - 12*lineheight,
        1.0f
    );

    //  Rotação
    char letterRotation_buffer[128];

    snprintf(
        letterRotation_buffer,
        sizeof(letterRotation_buffer),
        "Letter Rot: (%.1f %.1f %.1f)",
        g_DebugLetterRotation.x,
        g_DebugLetterRotation.y,
        g_DebugLetterRotation.z
    );

    TextRendering_PrintString(
        window,
        letterRotation_buffer,
        -1.0f + charwidth,
        1.0f - 13*lineheight,
        1.0f
    );


    //  Depuração: tempo de execução dos métodos da app. principal

    char updateDayNight_buffer[128];
    char sceneUpdate_buffer[128];
    char sceneResolveCollisions_buffer[128];
    char rendererBeginFrame_buffer[128];
    char rendererDraw_buffer[128];
    char rendererEndFrame_buffer[128];

    snprintf(
        updateDayNight_buffer,
        sizeof(updateDayNight_buffer),
        "Application::updateDayNight(): %.3f",
        g_debug_updateDayNight
    );

    snprintf(
        sceneUpdate_buffer,
        sizeof(sceneUpdate_buffer),
        "Scene::update(): %.3f",
        g_debug_sceneUpdate
    );

    snprintf(
        sceneResolveCollisions_buffer,
        sizeof(sceneResolveCollisions_buffer),
        "Scene::resolveCollisions(): %.3f",
        g_debug_sceneResolveCollisions
    );

    snprintf(
        rendererBeginFrame_buffer,
        sizeof(rendererBeginFrame_buffer),
        "Renderer::beginFrame(): %.3f",
        g_debug_rendererBeginFrame
    );

    snprintf(
        rendererDraw_buffer,
        sizeof(rendererDraw_buffer),
        "Renderer::draw(): %.3f",
        g_debug_rendererDraw
    );

    snprintf(
        rendererEndFrame_buffer,
        sizeof(rendererEndFrame_buffer),
        "Renderer::endFrame(): %.3f",
        g_debug_rendererEndFrame
    );

    TextRendering_PrintString(
        window,
        updateDayNight_buffer,
        -1.0f + charwidth,
        1.0f - 15*lineheight,
        1.0f
    );

    TextRendering_PrintString(
        window,
        sceneUpdate_buffer,
        -1.0f + charwidth,
        1.0f - 16*lineheight,
        1.0f
    );

    TextRendering_PrintString(
        window,
        sceneResolveCollisions_buffer,
        -1.0f + charwidth,
        1.0f - 17*lineheight,
        1.0f
    );

    TextRendering_PrintString(
        window,
        rendererBeginFrame_buffer,
        -1.0f + charwidth,
        1.0f - 18*lineheight,
        1.0f
    );

    TextRendering_PrintString(
        window,
        rendererDraw_buffer,
        -1.0f + charwidth,
        1.0f - 19*lineheight,
        1.0f
    );

    TextRendering_PrintString(
        window,
        rendererEndFrame_buffer,
        -1.0f + charwidth,
        1.0f - 20*lineheight,
        1.0f
    );
    
    TextRendering_ShowFramesPerSecond(window);
}


// Função callback chamada sempre que o usuário aperta algum dos botões do mouse
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        // Se o usuário pressionou o botão esquerdo do mouse, guardamos a
        // posição atual do cursor nas variáveis g_LastCursorPosX e
        // g_LastCursorPosY.  Também, setamos a variável
        // g_LeftMouseButtonPressed como true, para saber que o usuário está
        // com o botão esquerdo pressionado.
        glfwGetCursorPos(window, &g_LastCursorPosX, &g_LastCursorPosY);
        g_LeftMouseButtonPressed = true;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        // Quando o usuário soltar o botão esquerdo do mouse, atualizamos a
        // variável abaixo para false.
        g_LeftMouseButtonPressed = false;
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        // Se o usuário pressionou o botão esquerdo do mouse, guardamos a
        // posição atual do cursor nas variáveis g_LastCursorPosX e
        // g_LastCursorPosY.  Também, setamos a variável
        // g_RightMouseButtonPressed como true, para saber que o usuário está
        // com o botão esquerdo pressionado.
        glfwGetCursorPos(window, &g_LastCursorPosX, &g_LastCursorPosY);
        g_RightMouseButtonPressed = true;
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
    {
        // Quando o usuário soltar o botão esquerdo do mouse, atualizamos a
        // variável abaixo para false.
        g_RightMouseButtonPressed = false;
    }
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
    {
        // Se o usuário pressionou o botão esquerdo do mouse, guardamos a
        // posição atual do cursor nas variáveis g_LastCursorPosX e
        // g_LastCursorPosY.  Também, setamos a variável
        // g_MiddleMouseButtonPressed como true, para saber que o usuário está
        // com o botão esquerdo pressionado.
        glfwGetCursorPos(window, &g_LastCursorPosX, &g_LastCursorPosY);
        g_MiddleMouseButtonPressed = true;
    }
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE)
    {
        // Quando o usuário soltar o botão esquerdo do mouse, atualizamos a
        // variável abaixo para false.
        g_MiddleMouseButtonPressed = false;
    }
}

// Função callback chamada sempre que o usuário movimentar o cursor do mouse em
// cima da janela OpenGL.
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    // Abaixo executamos o seguinte: caso o botão esquerdo do mouse esteja
    // pressionado, computamos quanto que o mouse se movimento desde o último
    // instante de tempo, e usamos esta movimentação para atualizar os
    // parâmetros que definem a posição da câmera dentro da cena virtual.
    // Assim, temos que o usuário consegue controlar a câmera.

    if (g_LeftMouseButtonPressed)
    {
        // Deslocamento do cursor do mouse em x e y de coordenadas de tela!
        float dx = xpos - g_LastCursorPosX;
        float dy = ypos - g_LastCursorPosY;
    
        // Atualizamos parâmetros da câmera com os deslocamentos
        g_CameraTheta -= 0.01f*dx;
        g_CameraPhi   += 0.01f*dy;
    
        // Em coordenadas esféricas, o ângulo phi deve ficar entre -pi/2 e +pi/2.
        float phimax = 3.141592f/2;
        float phimin = -phimax;
    
        if (g_CameraPhi > phimax)
            g_CameraPhi = phimax;
    
        if (g_CameraPhi < phimin)
            g_CameraPhi = phimin;
    
        // Atualizamos as variáveis globais para armazenar a posição atual do
        // cursor como sendo a última posição conhecida do cursor.
        g_LastCursorPosX = xpos;
        g_LastCursorPosY = ypos;
    }

    if (g_RightMouseButtonPressed)
    {
        // Deslocamento do cursor do mouse em x e y de coordenadas de tela!
        float dx = xpos - g_LastCursorPosX;
        float dy = ypos - g_LastCursorPosY;

        // Atenção: o controle de voo do Bird é feito pela classe Bird::update().
        // O arrasto com o botão direito não altera diretamente a posição do modelo.

        // Atualizamos as variáveis globais para armazenar a posição atual do
        // cursor como sendo a última posição conhecida do cursor.
        g_LastCursorPosX = xpos;
        g_LastCursorPosY = ypos;
    }

    if (g_MiddleMouseButtonPressed)
    {
        // Deslocamento do cursor do mouse em x e y de coordenadas de tela!
        float dx = xpos - g_LastCursorPosX;
        float dy = ypos - g_LastCursorPosY;
        
        // Atualizamos as variáveis globais para armazenar a posição atual do
        // cursor como sendo a última posição conhecida do cursor.
        g_LastCursorPosX = xpos;
        g_LastCursorPosY = ypos;
    }
}

// Função callback chamada sempre que o usuário movimenta a "rodinha" do mouse.
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    // Atualizamos a distância da câmera para a origem utilizando a
    // movimentação da "rodinha", simulando um ZOOM.
    g_CameraDistance -= 0.1f*yoffset;

    // Uma câmera look-at nunca pode estar exatamente "em cima" do ponto para
    // onde ela está olhando, pois isto gera problemas de divisão por zero na
    // definição do sistema de coordenadas da câmera. Isto é, a variável abaixo
    // nunca pode ser zero. Versões anteriores deste código possuíam este bug,
    // o qual foi detectado pelo aluno Vinicius Fraga (2017/2).
    const float verysmallnumber = std::numeric_limits<float>::epsilon();
    if (g_CameraDistance < verysmallnumber)
        g_CameraDistance = verysmallnumber;
}

