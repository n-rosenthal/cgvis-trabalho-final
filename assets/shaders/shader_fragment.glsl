#version 330 core

// Atributos de fragmentos recebidos como entrada ("in") pelo Fragment Shader.
in vec3 fragPosition;
in vec3 fragNormal;

in vec2 fragTexcoord;
in vec3 fragColor;

// Matrizes computadas no código C++ e enviadas para a GPU
uniform mat4 view;

// Identificador que define qual objeto está sendo desenhado no momento
uniform int object_id;

// Parâmetros da axis-aligned bounding box (AABB) do modelo
uniform vec4 bbox_min;
uniform vec4 bbox_max;

// Variáveis para acesso das imagens de textura
uniform sampler2D TextureImage0;
uniform sampler2D TextureImage1;
uniform sampler2D TextureImage2;
uniform sampler2D TextureImage3;
uniform sampler2D TextureImage4;

// O valor de saída ("out") de um Fragment Shader é a cor final do fragmento.
out vec4 color;

// IDs iguais aos do enum C++
#define SPHERE      0
#define BUNNY       1
#define PLANE       2
#define TREE        3
#define BIRD        4
#define BIRD2       5
#define TARGET      6
#define ROCK        7
#define RING        8
#define BUILDING    9
#define LETTER      10

// Constantes
#define M_PI   3.14159265358979323846
#define M_PI_2 1.57079632679489661923

void main()
{
    vec3 N =
        normalize(fragNormal);

    vec3 L =
        normalize(vec3(
            1.0,
            1.0,
            0.5
        ));

    vec3 cameraPosition =
        vec3(inverse(view)[3]);

    vec3 viewDir =
        normalize(
            cameraPosition -
            fragPosition
        );

    // Coordenadas de textura
    float U = fragTexcoord.x;
    float V = fragTexcoord.y;

    // Cor difusa do material
    vec3 Kd0 = vec3(1.0);

    // ==================================
    // TERRENO
    // ==================================

    if(object_id == PLANE)
    {
        float ambient =
            0.25;

        float diffuse =
            max(dot(N,L),0.0);

        vec3 finalColor =
            fragColor *
            (ambient + diffuse);

        color =
            vec4(finalColor,1.0);

        return;
    }

    // ==================================
    // ÁRVORES
    // ==================================

    else if(object_id == TREE)
    {
        vec4 texColor = texture(TextureImage2, vec2(U,V));
        if (texColor.a < 0.3) {
            discard;
        }
        Kd0 = texColor.rgb;
    }

    // ==================================
    // PÁSSARO
    // ==================================

    else if(object_id == BIRD)
    {
        Kd0 =
            texture(
                TextureImage3,
                vec2(U,V)
            ).rgb;
    }

    else if(object_id == BIRD2)
    {
        Kd0 =
            texture(
                TextureImage4,
                vec2(U,V)
            ).rgb;
    }

    // ==================================
    // TARGET
    // ==================================

    else if(object_id == TARGET)
    {
        vec2 uv =
            fragTexcoord * 2.0 - 1.0;

        float r =
            length(uv);

        float ringPattern =
            sin(r * 40.0);

        if(ringPattern > 0.0)
        {
            Kd0 =
                vec3(
                    1.0,
                    0.0,
                    0.0
                );
        }
        else
        {
            Kd0 =
                vec3(
                    1.0,
                    1.0,
                    1.0
                );
        }
    }

    // ==================================
    // ROCHA
    // ==================================

    else if(object_id == ROCK)
    {
        float diffuse =
            max(dot(N,L),0.2);

        color =
            vec4(
                vec3(
                    0.35,
                    0.35,
                    0.38
                ) * diffuse,
                1.0
            );

        return;
    }

    // ==================================
    // ANEL
    // ==================================

    else if(object_id == RING)
    {
        float fresnel =
            pow(
                1.0 -
                max(dot(N,viewDir),0.0),
                3.0
            );

        vec3 emissive =
            vec3(
                1.0,
                0.1,
                0.1
            ) *
            (2.0 + 4.0*fresnel);

        color =
            vec4(emissive,1.0);

        return;
    }

    // ==================================
    // ESFERA
    // ==================================

    else if(object_id == SPHERE)
    {
        Kd0 =
            texture(
                TextureImage0,
                vec2(U,V)
            ).rgb;
    }

    // ==================================
    // COELHO
    // ==================================

    else if(object_id == BUNNY)
    {
        Kd0 =
            vec3(
                1.0,
                1.0,
                1.0
            );
    }

    // ==================================
    // CARTA
    // ==================================

    else if(object_id == LETTER)
    {
        Kd0 =
            vec3(
                0.9,
                0.85,
                0.75
            );
    }

    // ==================================
    // eq. iluminação
    // ==================================

    float lambert =
        max(
            dot(N,L),
            0.0
        );

    color.rgb =
        Kd0 *
        (lambert + 0.01);

    // Alpha default = 1 = 100% opaco = 0% transparente
    color.a = 1.0;

    // Cor final com correção gamma, considerando monitor sRGB.
    color.rgb =
        pow(
            color.rgb,
            vec3(1.0/2.2)
        );
}