# 📌 PROMPTS UTILIZADOS NO PROJETO

Este arquivo registra os prompts utilizados para gerar código com auxílio de IA, conforme requisito da atividade.


## Prompt #1

**Data:** 14/05/2026  
**Autor:** Joana  
**Objetivo:** Criar funcionalidade de painel na tela do sistema

**Prompt:**
```text
Criar painel de interface do usuário/depuração exibindo métricas de voo em tempo real.
```

**Arquivos gerados/afetados:**

* `main.cpp`

**Observações/Resposta:**

* Painel de depuração criado com sucesso! O painel exibe em tempo real:

* **Posição do bunny**: X, Y, Z

* **Rotação**: em radianos e graus

* **Hora atual**: formato HH:MM:SS com status DIA/NOITE

* **Parâmetros da câmera**: distância, theta, phi

* **Instruções de controle**: lista de teclas disponíveis

**Controles:**

* **T**: Liga/desliga o painel de depuração
* **H**: Liga/desliga o texto informativo padrão

O painel aparece no canto superior esquerdo da tela e atualiza automaticamente a cada frame.

***
## Prompt #2
**Data:** 19/05/2026  
**Autor:** Joana  
**Objetivo:** Corrigir texturas

**Prompt:**

```text
Identifique os problemas que contêm nos shaders e nas texturas. Por exemplo, a tree não está utilizando a textura própria, mas a do tijolo.
```

**Arquivos gerados/afetados:**

* Nenhum arquivo foi afetado; apenas problemas foram identificados.

**Observações:**

***

## Problemas Identificados nos Shaders e Texturas

**1. Árvore usando textura de tijolo**

* **Localização**: `engine/src/main.cpp`, linha 506
* **Problema**: `g_Tree.draw(g_model_uniform, g_object_id_uniform, SPHERE);`
* **Causa**: Árvores são desenhadas com `object_id = SPHERE`, que no shader usa `TextureImage0` (tijolo vermelho)
* **Textura correta disponível**: `assets/textures/tree1_textures/Gentree_2_Twigs_AE3D_04022023-B-DIFFUSE.jpg` (não carregada)

**2. Bunny usando textura de tijolo**

* **Localização**: `shader_fragment.glsl`, linha 126
* **Problema**: BUNNY usa `TextureImage0` (tijolo) em vez de textura própria
* **Causa**: Linha 126: `Kd0 = texture(TextureImage0, vec2(U,V)).rgb;`

**3. Shader fragment incompleto**

* **Localização**: `shader_fragment.glsl`, linhas 22–24
* **Problema**: Apenas define SPHERE (0), BUNNY (1), PLANE (2)
* **Falta**: Não há casos para TREE ou BIRD
* **Impacto**: Objetos sem case específico não têm textura definida

**4. Textura de árvore não carregada**

* **Localização**: `engine/src/main.cpp`, linhas 352–353
* **Problema**: Apenas 2 texturas carregadas (tijolo e terreno)
* **Falta**: A textura da árvore não é carregada

**5. TextureImage2 não utilizado**

* **Localização**: `shader_fragment.glsl`, linha 34
* **Problema**: `TextureImage2` está definido, mas não é usado
* **Causa**: Nenhuma textura foi carregada para `TextureImage2`

**Soluções necessárias:**

1. Carregar a textura da árvore como `TextureImage2`
2. Adicionar o caso TREE no shader fragment usando `TextureImage2`
3. Adicionar o caso BIRD no shader fragment
4. Definir uma textura própria para BUNNY ou usar outra textura existente

***

## Prompt #3

**Data:** 25/05/2026  
**Autor:** Joana D'Avila  
**Objetivo:** Fazer ajustes finos

**Prompt:**

```text
I want you to make it so that I can pick up the letter when I press space, and if the bird is flying when space is pressed, the letter is dropped.
Add a space on the plane where the target should be: a circle with a red border and a white inner circle. Also, add to the information panel the distance between the letter and the target.
The information panel should be divided into: information about location, time, etc., and controls. Controls must be displayed when pressing M, in the middle of the screen like a popup, and the bird speed must be stopped, working like a pause.
The information must be shown or hidden when pressing I, on the upper left side of the screen.
```

**Arquivos gerados/afetados:**
main.cpp (implementação da lógica de controle, captura e pause)
shader_fragment.glsl (possível uso para renderização do target, se aplicável)

**Observações:**
Funcionalidades implementadas:


✅ Captura da letter com Space

Pressione Space quando o bird estiver próximo da letter (distância < 2.0) para capturar
Pressione Space novamente quando a letter estiver capturada para soltá-la



✅ Target no plano

Target posicionado em (0.0, -1.0, 0.0) no plano
Utiliza uma esfera como base (pode ser aprimorado para um círculo vermelho com interior branco)



✅ Distância Letter–Target

Calculada dinamicamente e exibida no painel de informações



✅ Variáveis de controle

g_ShowInfoPanel: controla a exibição do painel de informações
g_ShowControlsPopup: controla o popup de controles
g_IsPaused: indica se o jogo está pausado



✅ Tecla M — Popup de Controles

Exibe um popup central na tela com os controles
Pausa automaticamente o jogo ao ser ativado



✅ Tecla I — Toggle do Info Panel

Mostra/esconde o painel de informações no canto superior esquerdo



✅ Lógica de pausa

Quando pausado, o bird e a letter não atualizam sua física



✅ Separação de painéis

Info Panel: posição do bird/letter, distância, rotação, clima, relógio, estado e câmera
Controls Popup: lista de controles (W, S, A, D, Q, E, Space, F, L, P, O, R, M, I, ESC)

***

## Prompt #n
**Data:** DD/MM/AAAA  
**Autor:** Nome  
**Objetivo:** ...

**Prompt:**

```text
Seu prompt aqui
```

**Arquivos gerados/afetados:**

*   ...

**Observações:**
*   ...
