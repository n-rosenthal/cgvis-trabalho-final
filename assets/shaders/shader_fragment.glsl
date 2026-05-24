#version 330 core

in vec4 position_world;
in vec4 normal;
in vec4 position_model;
in vec2 texcoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

#define SPHERE 0
#define BUNNY  1
#define PLANE  2
#define ROCK   4

uniform int object_id;

uniform vec4 bbox_min;
uniform vec4 bbox_max;

uniform sampler2D TextureImage0;
uniform sampler2D TextureImage1;
uniform sampler2D TextureImage2;

out vec4 color;

#define M_PI   3.14159265358979323846
#define M_PI_2 1.57079632679489661923

void main()
{
    vec4 origin = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 camera_position = inverse(view) * origin;

    vec4 p = position_world;

    // Normal interpolada
    vec4 n = normalize(normal);

    // Direção da luz
    vec4 l = normalize(vec4(1.0, 1.0, 0.5, 0.0));

    // Vetor para câmera
    vec4 v = normalize(camera_position - p);

    float U = 0.0;
    float V = 0.0;

    vec3 Kd0 = vec3(1.0);

    // ===== ESFERA =====
    if ( object_id == SPHERE )
    {
        vec4 bbox_center = (bbox_min + bbox_max) / 2.0;
        vec4 d = position_model - bbox_center;

        float rho   = length(d.xyz);
        float theta = atan(d.x, d.z);
        float phi   = asin(d.y / rho);

        U = (theta + M_PI) / (2.0 * M_PI);
        V = (phi + M_PI_2) / M_PI;

        Kd0 = texture(TextureImage0, vec2(U,V)).rgb;
    }

    // ===== COELHO =====
    else if ( object_id == BUNNY )
    {
        float minx = bbox_min.x;
        float maxx = bbox_max.x;

        float miny = bbox_min.y;
        float maxy = bbox_max.y;

        U = (position_model.x - minx) / (maxx - minx);
        V = (position_model.y - miny) / (maxy - miny);

        Kd0 = texture(TextureImage0, vec2(U,V)).rgb;
    }

    // ===== TERRENO =====
    else if ( object_id == PLANE )
    {
        color.rgb = vec3(0.2, 0.7, 0.2);
        color.a = 1.0;
        return;
    }

    // ===== ROCHAS =====
    else if (object_id == ROCK)
    {
        vec3 N = normalize(n.xyz);
        vec3 L = normalize(l.xyz);

        float diffuse = max(dot(N, L), 0.2);

        vec3 rockColor =
            vec3(0.35, 0.35, 0.38);

        color.rgb = rockColor * diffuse;
        color.a = 1.0;
        return;
    }

    // ===== ILUMINAÇÃO LAMBERT =====

    float lambert = max(dot(n.xyz, l.xyz), 0.0);

    vec3 ambient = 0.15 * Kd0;
    vec3 diffuse = lambert * Kd0;

    vec3 final_color = ambient + diffuse;

    color = vec4(final_color, 1.0);

    // Gamma correction
    color.rgb = pow(color.rgb, vec3(1.0/2.2));
}

