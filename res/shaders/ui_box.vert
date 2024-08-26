#version 460 core

layout (location = 0) in vec2 aPos;

uniform vec2 uPos;
uniform vec2 uSize;

out vec2 vTexCoord;

void main()
{
    vec2 pos_norm = aPos * 0.5 + 0.5;

    vTexCoord = pos_norm;
    
    vec2 pos = pos_norm * uSize + uPos;

    gl_Position = vec4(pos * 2.0 - 1.0, 0.0, 1.0);
}
