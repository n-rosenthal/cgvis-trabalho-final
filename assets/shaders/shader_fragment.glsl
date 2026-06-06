#version 330 core

in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTexcoord;
in vec3 fragColor;

uniform mat4 view;

uniform int object_id;

uniform vec4 bbox_min;
uniform vec4 bbox_max;

uniform sampler2D TextureImage0;
uniform sampler2D TextureImage1;
uniform sampler2D TextureImage2;

out vec4 color;

#define SPHERE 0
#define BUNNY  1
#define PLANE  2
#define BIRD   3
#define ROCK   4
#define RING   5

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

    vec3 V =
        normalize(
            cameraPosition -
            fragPosition
        );

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
    // ROCHA
    // ==================================

    if(object_id == ROCK)
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

    if(object_id == RING)
    {
        float fresnel =
            pow(
                1.0 -
                max(dot(N,V),0.0),
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
    // OBJETOS COM TEXTURA
    // ==================================

    vec3 Kd0 =
        texture(
            TextureImage0,
            fragTexcoord
        ).rgb;

    float lambert =
        max(dot(N,L),0.0);

    vec3 ambient =
        0.15 * Kd0;

    vec3 diffuse =
        lambert * Kd0;

    color =
        vec4(
            ambient + diffuse,
            1.0
        );

    color.rgb =
        pow(
            color.rgb,
            vec3(1.0/2.2)
        );
}