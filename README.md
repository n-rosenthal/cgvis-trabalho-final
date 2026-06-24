# Trabalho Final de Computação Gráfica: *Bird Carrier Simulator*

![./docs/imagens/bird-carrier-simulator-1.png]

## Descrição da Aplicação
O projeto consiste em um jogo 3D desenvolvido em C++ utilizando OpenGL. O jogador controla um pássaro em um ambiente virtual composto por terreno, vegetação, rochas e outros elementos da cena.
O objetivo é explorar o cenário, atravessar anéis espalhados pelo mapa e entregar a carta em sua destinação final. 
O projeto utiliza conceitos fundamentais de Computação Gráfica, incluindo modelagem, transformações geométricas, iluminação, texturização, sistemas de colisões e renderização em tempo real.

---

## Contribuições da Dupla
Fases de desenvolvimento e aspectos do projeto implementados pela dupla:

* Arquitetura geral do projeto;
* Implementação da engine gráfica;
* Sistema de renderização;
* Sistema de colisões;
* Sistemas de billboards e de partículas;
* Terreno procedural;
* Implementação dos objetos da cena;
* Sistema de animações;
* Documentação técnica;
* Correções e adequação às restrições da disciplina.
* Modelagem e integração de assets;
* Seleção e tratamento de texturas;
* Construção de elementos da cena;
* Testes de jogabilidade;
* Ajustes visuais; e
* Documentação e validação do projeto.

---

## Imagens

### Cena principal

![Cena principal](images/gameplay1.png)

### Navegação pelo cenário

![Gameplay](images/gameplay2.png)

---
![./docs/imagens/bird-carrier-simulator-2.png]

## Manual de Utilização

### Objetivo

Controlar o pássaro através do cenário, atravessar os anéis distribuídos pelo mapa e localizar a levar a carta de sua posição inicial até a caixa do correio, que se encontra ao final do caminho dos anéis. Arremessar a carta no local esperado.

### Controles

| Tecla  | Função             |
|--------|--------------------|
| W      | Avançar            |
| S      | Recuar             |
| A      | Virar à esquerda   |
| D      | Virar à direita    |
| Espaço | Subir              |
| Shift  | Descer             |
| G      | Soltar a carta     |
| Esc    | Encerrar aplicação |

> Ajuste a tabela acima para refletir exatamente os controles implementados no projeto.

---

![./docs/imagens/bird-carrier-simulator-3.png]

## Tecnologias Utilizadas

* C++
* Blender
* OpenGL 3.3+
* GLFW
* GLM
* stb_image
* TinyObjLoader
* CMake

---

## Compilação
### Dependências
* Compilador C++ com suporte a C++17
* CMake
* OpenGL
* GLFW
* GLM

### Linux

```bash
git clone https://github.com/n-rosenthal/cgvis-trabalho-final.git
cd cgvis-trabalho-final

mkdir build
cd build

cmake ..
make -j$(nproc)
```

### Windows

```bash
git clone https://github.com/n-rosenthal/cgvis-trabalho-final.git
cd cgvis-trabalho-final

mkdir build
cd build

cmake ..
cmake --build . --config Release
```

---

## Execução

Após a compilação:

```bash
./build/main
```

ou o executável correspondente gerado pelo sistema operacional.

---

![./docs/imagens/bird-carrier-simulator-4.png]


## Funcionalidades Implementadas

* Renderização de modelos tridimensionais através de OpenGL e com amplo uso das técnicas de Computação Gráfica em geral, e Álgebra Linear em específico, vistas em sala de aula;
* Terreno procedural baseado em funções matemáticas, ruídoss, determinação de texturas a partir de altitude ou distância a um ponto;
* Objetos estáticos e dinâmicos, animados em função do tempo;
* Sistema de colisões com diversos tipos de colisores distintos entre si (colisão por *bounding boxes*, *colisão capsular*, *colisão esférica* e *colisão cilíndrica*;
* Skybox com *shaders* injetados dinamicamente em tempo de execução;
* Billboards animados;
* Iluminação básica;
* Texturização para todos os modelos e para todos os pontos do terreno (novamente, em função da altitude, distãncia do lago central, com degradê de transição entre texturas);
* Sistema de partículas (emissão de *trails*, *bursts* ou explosões de partículas);
* Interação do personagem principal com outros objetos do jogo, seja através de colisões, seja através do carregamento da carta até a caixa de correio, que marca o final do jogo.
* Conversão de diversos tipos de formatos de *meshes* para Wavefront OBJ;
* Curvas de Bézier para determinação do percurso dos NPCs no jogo;
* Interface de jogo básica, com tela de carregamento (enquanto são carregadas as texturas, modelos, shaders etc), menu principal simples; e
* Lógica de jogo razoável, ainda que simples, com início, meio e fim.
