#version 330 core

in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTexcoord;
in vec3 fragColor;

uniform mat4 view;
uniform int object_id;

uniform vec4 bbox_min;
uniform vec4 bbox_max;

uniform sampler2D diffuseTexture;
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

void main()
{
    // =====================================================
    // Iluminação básica
    // =====================================================

    vec3 N =
        normalize(fragNormal);

    vec3 L =
        normalize(
            vec3(
                1.0,
                1.0,
                0.5
            )
        );

    float lambert =
        max(
            dot(N, L),
            0.0
        );

    float ambient = 0.10;

    vec3 Kd0;

    // =====================================================
    // Terreno
    // =====================================================

    if(object_id == PLANE)
    {
        Kd0 = fragColor;

        color =
            vec4(
                Kd0 * (ambient + lambert),
                1.0
            );

        color.rgb =
            pow(
                color.rgb,
                vec3(1.0 / 2.2)
            );

        return;
    }

    // =====================================================
    // Objetos procedurais
    // (rochas e anéis)
    // =====================================================

    if(object_id == ROCK) {
        Kd0 = fragColor;

        color =
            vec4(
                Kd0 * (ambient + lambert),
                1.0
            );

        color.rgb =
            pow(
                color.rgb,
                vec3(1.0 / 2.2)
            );

        return;
    }

    if(object_id == RINGS) {
        float diffuse = max(dot(N,L), 0.2);

        color =
            vec4(
                vec3(1.0, 0.15, 0.15) * diffuse,
                1.0
            );

        return;
    }

    // =====================================================
    // Objetos texturizados
    // =====================================================

    if(useTexture)
    {
        vec4 texColor =
            texture(
                diffuseTexture,
                fragTexcoord
            );

        if(texColor.a < 0.3)
            discard;

        Kd0 = texColor.rgb;
    }
    else
    {
        Kd0 = fragColor;
    }

    // =====================================================
    // Cor final
    // =====================================================

    color =
        vec4(
            Kd0 * (ambient + lambert),
            1.0
        );

    color.rgb =
        pow(
            color.rgb,
            vec3(1.0 / 2.2)
        );
}