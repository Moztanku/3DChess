#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 uModel;
uniform mat4 uMVP;

out vec3 normal;
out vec3 fragPos;

void main()
{
    fragPos = vec3(uModel * vec4(aPos, 1.0));
    normal = mat3(transpose(inverse(uModel))) * aNormal;

    gl_Position = uMVP * vec4(aPos, 1.0);
}
