//     Universidade Federal do Rio Grande do Sul
//             Instituto de Informática
//       Departamento de Informática Aplicada
//
//    INF01047 Computação Gráfica e Visualização I
//               Prof. Eduardo Gastal
//
//     CÓDIGO BASE PARA O TRABALHO FINAL
//

// Arquivos "headers" padrões de C podem ser incluídos em um
// programa C++, sendo necessário somente adicionar o caractere
// "c" antes de seu nome, e remover o sufixo ".h". Exemplo:
//    #include <stdio.h> // Em C
//  vira
//    #include <cstdio> // Em C++
//

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>

// Headers abaixo são específicos de C++
#include <set>
#include <map>
#include <stack>
#include <string>
#include <vector>
#include <limits>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>

// Headers das bibliotecas OpenGL
#include <glad/glad.h>   // Criação de contexto OpenGL 3.3
#include <GLFW/glfw3.h>  // Criação de janelas do sistema operacional
#include <GL/gl.h>

// Headers da biblioteca GLM: criação de matrizes e vetores.
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp> // glm::lookAt, glm::translate, etc.

// Headers da biblioteca para carregar modelos obj
#include <tiny_obj_loader.h>
#include <stb_image.h>

// Headers locais, definidos na pasta "include/"
#include "utils.h"
#include "matrices.h"
#include "Renderer/ShaderLoader.hpp"
#include "Window/WindowCallbacks.hpp"
#include "Window/InputCallbacks.hpp"
#include "textrendering.hpp"
#include "Game/AssetLoader.hpp"

// ÁUDIO e gerenciador de áudio
#include "audio/AudioManager.hpp"
SoundManager g_Sound;

//  GLFW, GLAD, inicialização da aplicação gráfica
#include "Game/Window.hpp"


//  Loader de modelos Wavefront .obj
#include "Loaders/ObjLoader.hpp"
#include "Loaders/TextureLoader.hpp"


//  CÂMERA
#include "Game/Camera.hpp"
Camera g_Camera;

//  Classe `Bird`: ave controlada pelo usuário
#include "Game/Bird.hpp"

// Classe `Tree`: árvores geradas aleatoriamente
//                -> StaticObject

// Classe `ProceduralTerrain`: terreno gerado proceduralmente
#include "Terrain/Terrain.hpp"

// Classes `ProceduralRock`: rochas geradas proceduralmente
#include "Objects/ProceduralRock.hpp"

// Classe `Ring`: anéis através dos quais o pássaro deve voar
#include "Objects/Ring.hpp"

// Class `Building`: construções/edifícios retangulares
#include "Objects/Building.hpp"

//  Sistema de Colisões
#include "Collision/CollisionSystem.hpp"


#include "Game/Application.hpp"

int main(int argc, char* argv[]) {
    Application app;
    app.init("Bird Carrier - Joana & Nicolas", 800, 600);
    app.run();
    return 0;
}


// set makeprg=cd\ ..\ &&\ make\ run\ >/dev/null
// vim: set spell spelllang=pt_br :

