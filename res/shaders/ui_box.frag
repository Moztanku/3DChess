#version 460 core

in vec2 vTexCoord;

uniform sampler2D uTexture;

uniform float uAlpha;
uniform vec3 uColor;

out vec4 fragColor;

void main()
{
    vec4 texColor = texture(uTexture, vTexCoord);
    fragColor = vec4(uColor, 1.0) * texColor * uAlpha;
}
