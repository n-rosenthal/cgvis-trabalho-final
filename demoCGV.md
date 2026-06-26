jun. 25, 2026
Reunião em 25 de jun. de 2026 às 20:58 GMT-03:00
Registros da reunião Gravação 


Resumo
Apresentação do projeto final de computação gráfica detalhou mecânicas de voo, geração procedural e sistemas de colisão.

Conceito e objetivos iniciais
O trabalho final propõe um jogo inspirado em Harry Potter com foco no controle da coruja Edwiges. O objetivo principal consiste em atravessar arcos e entregar uma correspondência.

Implementação técnica e terreno
O desenvolvimento incluiu animações de personagens via curvas de Bézier e geração procedural de terreno. Texturas variam conforme a altitude e localização no mapa.

Colisões e mecânicas finais
Sistemas de colisão diferenciados permitem pousos e interação física com objetos. A mecânica de arremesso da carta conclui o jogo após a coleta dos anéis.


Detalhes
Apresentação do Trabalho Final - Conceito e Objetivos: Joana Oliveira (Jo) e Nícolas Dal Corso apresentam o trabalho final da disciplina de computação gráfica e visualização. A proposta consiste em um jogo inspirado em um título antigo do Harry Potter, no qual o jogador controla a coruja Edwiges. Os objetivos estabelecidos para o jogador incluem atravessar arcos e transportar uma carta até uma caixa de correspondência.
Implementação de Áudio, Modelos e Animação de NPCs: Nícolas Dal Corso detalha a integração de um sistema de som para momentos específicos da jogabilidade e a criação de dois modelos distintos para a ave, representando a personagem em repouso e em voo. Adicionalmente, foram aplicadas curvas de Bézier para realizar as animações de NPCs, especificamente pássaros e borboletas.
Geração Procedural de Terreno e Texturização: Nícolas Dal Corso explica que o terreno é gerado proceduralmente por meio de funções que controlam parâmetros como as dimensões do lago central, campos e cadeias de montanhas. As texturas são aplicadas conforme a localização no mapa, variando entre água e areia, grama, rochas em zonas de transição e neve em altitudes mais elevadas.
Sistemas de Colisão: Nícolas Dal Corso descreve a implementação de colisões utilizando diferentes geometrias: esferas para copas de árvores e arbustos; cilindros para troncos e rochas; e caixas delimitadoras para a casa no final do percurso. O sistema diferencia a física dos objetos, permitindo a travessia de arbustos com redução de velocidade, enquanto troncos e rochas são barreiras sólidas. Também foi implementada a colisão com o chão, permitindo que a ave pouse.
Mecânicas de Interação e Conclusão do Jogo: Nícolas Dal Corso e Joana Oliveira (Jo) descrevem recursos adicionais, como o sistema de partículas, que cria explosões e rastros para a ave e a carta, e a inclusão de uma skybox. Os apresentadores detalham a mecânica de arremesso da carta, ativada pela tecla G, que projeta uma trajetória parabólica acompanhada pela câmera. O jogo é finalizado quando a carta é entregue na caixa de correspondência após a coleta de todos os anéis.