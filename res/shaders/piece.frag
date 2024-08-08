#version 460 core

in vec3 normal;
in vec3 fragPos;

uniform vec3 uColor;
uniform vec3 uLightDir;
uniform vec3 uLightColor;

out vec4 fragColor;

void main()
{
    vec3 norm = normalize(normal);

    float diff = max(dot(norm, uLightDir), 0.0) * 0.7 + 0.3;

    vec3 diffuse = diff * uLightColor;

    fragColor = vec4(uColor * diffuse, 1.0);
}
