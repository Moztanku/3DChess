#version 460 core

uniform sampler2D uTexture;
uniform float uTime;

in vec2 texCoords;

out vec4 fragColor;

float rand(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}


void main()
{
    vec2 zoom = texCoords * 0.9 + 0.05;
    vec4 color = texture(uTexture, zoom + vec2(0.05 * sin(uTime / 5.0), 0.05 * cos(uTime / 4.0)));
    vec4 negative = vec4(1.0 - color.r, 1.0 - color.g, 1.0 - color.b, 1.0);

    fragColor = mix(color, negative, sin(uTime / 1.0) * 0.5 + 0.5);
}
