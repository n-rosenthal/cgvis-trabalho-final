# 📌 PROMPTS UTILIZADOS NO PROJETO

Este arquivo registra os prompts utilizados para gerar código com auxílio de IA,
conforme requisito da atividade.

## Prompt #1

**Data:** 14/05/2026  
**Autor:** Joana  
**Objetivo:** Criar funcionalidade de painel na tela do sistema  

**Prompt:**
```text
Criar painel de interface do usuário/depuração exibindo métricas de voo em tempo real.
````

**Arquivos gerados/afetados:**
*   `main.cpp`

**Observações/Resposta:**

* Painel de depuração criado com sucesso! O painel exibe em tempo real:

- **Posição do bunny**: X, Y, Z
- **Rotação**: em radianos e graus
- **Hora atual**: formato HH:MM:SS com status DIA/NOITE
- **Parâmetros da câmera**: distância, theta, phi
- **Instruções de controle**: lista de teclas disponíveis

**Controles**:
- **T**: Liga/desliga o painel de depuração
- **H**: Liga/desliga o texto informativo padrão

O painel aparece no canto superior esquerdo da tela e atualiza automaticamente a cada frame.

***
## Prompt #2
**Data:** 19/05/2026  
**Autor:** Joana  
**Objetivo:** Corrigir texturas

**Prompt:**

```text
identifique os problemas que contem no shadders e texturas. por exemplo, tree não está utilizando a textura propria, mas a do tijolo
```

**Arquivos gerados/afetados:**

* nenhum arquivo afetado, apenas identificado problemas

**Observações:**
*   





















## Problemas Identificados nos Shaders e Texturas

**1. Árvore usando textura de tijolo**
- **Localização**: `engine/src/main.cpp` linha 506
- **Problema**: `g_Tree.draw(g_model_uniform, g_object_id_uniform, SPHERE);`
- **Causa**: Árvores são desenhadas com `object_id = SPHERE`, que no shader usa `TextureImage0` (tijolo vermelho)
- **Textura correta disponível**: [assets/textures/tree1_textures/Gentree_2_Twigs_AE3D_04022023-B-DIFFUSE.jpg](cci:7://file:///c:/dev/cgvis-trabalho-final/assets/textures/tree1_textures/Gentree_2_Twigs_AE3D_04022023-B-DIFFUSE.jpg:0:0-0:0) (não carregada)

**2. Bunny usando textura de tijolo**
- **Localização**: [shader_fragment.glsl](cci:7://file:///c:/dev/cgvis-trabalho-final/src/shader_fragment.glsl:0:0-0:0) linha 126
- **Problema**: BUNNY usa `TextureImage0` (tijolo) em vez de textura própria
- **Causa**: Linha 126: `Kd0 = texture(TextureImage0, vec2(U,V)).rgb;`

**3. Shader fragment incompleto**
- **Localização**: [shader_fragment.glsl](cci:7://file:///c:/dev/cgvis-trabalho-final/src/shader_fragment.glsl:0:0-0:0) linhas 22-24
- **Problema**: Apenas define SPHERE(0), BUNNY(1), PLANE(2)
- **Falta**: Não há casos para TREE ou BIRD
- **Impacto**: Objetos sem case específico não têm textura definida

**4. Textura de árvore não carregada**
- **Localização**: `engine/src/main.cpp` linhas 352-353
- **Problema**: Apenas 2 texturas carregadas (tijolo e terreno)
- **Falta**: Textura da árvore não é carregada

**5. TextureImage2 não utilizado**
- **Localização**: [shader_fragment.glsl](cci:7://file:///c:/dev/cgvis-trabalho-final/src/shader_fragment.glsl:0:0-0:0) linha 34
- **Problema**: `TextureImage2` está definido mas não é usado
- **Causa**: Nenhuma textura carregada para TextureImage2

**Soluções necessárias**:
1. Carregar textura da árvore como TextureImage2
2. Adicionar caso TREE no shader fragment usando TextureImage2
3. Adicionar caso BIRD no shader fragment
4. Definir textura própria para BUNNY ou usar outra textura existente

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
