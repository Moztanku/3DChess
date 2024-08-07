#version 460 core

uniform sampler2D uTexture;

in vec2 texCoord;

out vec4 fragColor;

void main()
{
    fragColor = texture(uTexture, texCoord);
}
