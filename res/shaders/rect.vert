#version 460 core

layout (location = 0) in vec2 aPos;

uniform mat4 uModel;
uniform mat4 uProjView;

void main()
{
    gl_Position = uProjView * uModel * vec4(aPos, 0.0, 1.0);
}
