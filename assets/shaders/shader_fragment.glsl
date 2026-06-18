#version 330 core

in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTexcoord;
in vec4 fragColor;             // agora vec4

uniform mat4 view;
uniform int  object_id;
uniform vec4 bbox_min;
uniform vec4 bbox_max;
uniform sampler2D diffuseTexture;
uniform sampler2D texSand;
uniform sampler2D texGrass;
uniform sampler2D texRock;
uniform sampler2D texSnow;
uniform bool useTexture;

out vec4 color;

#define SPHERE      0
#define BUNNY       1
#define PLANE       2
#define TREE        3
#define BIRD        4
#define BIRD2       5
#define TARGET      6
#define ROCK        7
#define RINGS       8
#define BUILDING    9
#define LETTER      10
#define WATER       11

void main()
{
    vec3  N       = normalize(fragNormal);
    vec3  L       = normalize(vec3(1.0, 1.0, 0.5));
    float lambert = max(dot(N, L), 0.0);
    float ambient = 0.10;
    vec3  Kd0;

    // =========================================================
    // Terreno
    // =========================================================
    if (object_id == PLANE)
    {
        // =====================================================
        // Texturas (cada material com sua própria escala)
        // =====================================================

        vec4 colSand  = texture(texSand , fragTexcoord * 40.0);
        vec4 colGrass = texture(texGrass, fragTexcoord * 30.0);
        vec4 colRock  = texture(texRock , fragTexcoord * 20.0);
        vec4 colSnow  = texture(texSnow , fragTexcoord * 15.0);

        float y = fragPosition.y;

        // =====================================================
        // Inclinação
        // 0 = plano
        // 1 = parede vertical
        // =====================================================

        float slope =
            1.0 -
            max(dot(N, vec3(0.0, 1.0, 0.0)), 0.0);

        // =====================================================
        // Transições de bioma por altitude
        // =====================================================

        float sandToGrass =
            smoothstep(-2.0, 4.0, y);

        float grassToRock =
            smoothstep(12.0, 35.0, y);

        float rockToSnow =
            smoothstep(55.0, 80.0, y);

        // =====================================================
        // Mistura principal
        // =====================================================

        vec3 terrainColor =
            mix(
                colSand.rgb,
                colGrass.rgb,
                sandToGrass
            );

        terrainColor =
            mix(
                terrainColor,
                colRock.rgb,
                grassToRock
            );

        terrainColor =
            mix(
                terrainColor,
                colSnow.rgb,
                rockToSnow
            );

        // =====================================================
        // Rocha automática em encostas
        // =====================================================

        float rockSlope =
            smoothstep(
                0.25,
                0.50,
                slope
            );

        terrainColor =
            mix(
                terrainColor,
                colRock.rgb,
                rockSlope * (1.0 - rockToSnow)
            );

        // =====================================================
        // Paredões muito íngremes
        // =====================================================

        float cliffRock =
            smoothstep(
                0.60,
                0.85,
                slope
            );

        terrainColor =
            mix(
                terrainColor,
                colRock.rgb,
                cliffRock
            );

        // =====================================================
        // Neve extra apenas nos picos
        // =====================================================

        float peakSnow =
            smoothstep(
                75.0,
                110.0,
                y
            );

        terrainColor =
            mix(
                terrainColor,
                colSnow.rgb,
                peakSnow
            );

        // =====================================================
        // Color grading procedural
        //
        // fragColor vem do CPU (sampleColor()).
        // Isso permite modificar a tonalidade do terreno
        // sem perder os detalhes das texturas.
        // =====================================================

        terrainColor *= fragColor.rgb;

        // =====================================================
        // Escurecimento suave por inclinação
        // Dá mais profundidade visual às montanhas.
        // =====================================================

        float slopeShade =
            mix(
                1.0,
                0.75,
                slope
            );

        terrainColor *= slopeShade;

        // =====================================================
        // Iluminação
        // =====================================================

        vec3 litColor =
            terrainColor *
            (ambient + lambert);

        color =
            vec4(
                litColor,
                1.0
            );

        // =====================================================
        // Correção gamma
        // =====================================================

        color.rgb =
            pow(
                max(color.rgb, vec3(0.0)),
                vec3(1.0 / 2.2)
            );

        return;
    }

    // =========================================================
    // Água  –  translúcida, iluminação suave, sem gamma agressivo
    // =========================================================
    if (object_id == WATER)
    {
        // Iluminação mais suave: lambert com mínimo alto para não escurecer
        float diff    = max(dot(N, L), 0.3);
        // Specular simples (Blinn-Phong) para brilho da superfície
        vec3  V       = normalize(-fragPosition);   // view space approx
        vec3  H       = normalize(L + V);
        float spec    = pow(max(dot(N, H), 0.0), 64.0) * 0.5;

        vec3 waterCol = fragColor.rgb * (0.15 + diff) + vec3(spec);
        // Alpha vem direto do vertex (gradiente centro→borda)
        float alpha   = fragColor.a;

        color = vec4(waterCol, alpha);
        // Gamma mais suave para a água (não gamma completo, fica menos saturado)
        color.rgb = pow(max(color.rgb, vec3(0.0)), vec3(1.0 / 1.8));
        return;
    }

    // =========================================================
    // Rochas e anéis
    // =========================================================
    if (object_id == ROCK)
    {
        Kd0   = fragColor.rgb;
        color = vec4(Kd0 * (ambient + lambert), 1.0);
        color.rgb = pow(color.rgb, vec3(1.0 / 2.2));
        return;
    }

    if (object_id == RINGS)
    {
        float diffuse = max(dot(N, L), 0.2);
        color = vec4(vec3(1.0, 0.15, 0.15) * diffuse, 1.0);
        return;
    }

    // =========================================================
    // Objetos texturizados / genéricos
    // =========================================================
    if (useTexture)
    {
        vec4 texColor = texture(diffuseTexture, fragTexcoord);
        if (texColor.a < 0.3) discard;
        Kd0 = texColor.rgb;
    }
    else
    {
        Kd0 = fragColor.rgb;
    }

    color     = vec4(Kd0 * (ambient + lambert), 1.0);
    color.rgb = pow(color.rgb, vec3(1.0 / 2.2));
}
