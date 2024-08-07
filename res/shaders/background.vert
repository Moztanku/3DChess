#version 460 core

layout (location = 0) in vec2 aPos;

out vec2 texCoords;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
    texCoords = (aPos + 1.0) / 2.0;
}
