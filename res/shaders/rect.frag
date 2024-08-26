#version 460 core

uniform vec4 uRGBA;

out vec4 fragColor;

void main()
{
    fragColor = uRGBA;
}