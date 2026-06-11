#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 vertex_normal;
layout(location = 2) in vec2 vertex_texcoord;
layout(location = 3) in vec4 vertex_color;   // vec4 agora (alpha usado pela água)

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 fragPosition;
out vec3 fragNormal;
out vec2 fragTexcoord;
out vec4 fragColor;            // passa vec4 pro fragment

void main()
{
    vec4 worldPos  = model * vec4(position, 1.0);
    fragPosition   = worldPos.xyz;
    fragNormal     = mat3(transpose(inverse(model))) * vertex_normal;
    fragTexcoord   = vertex_texcoord;
    fragColor      = vertex_color;
    gl_Position    = projection * view * worldPos;
}
