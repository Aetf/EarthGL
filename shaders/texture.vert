#version 430 core

uniform mat4 vProjection;
uniform mat4 vModelView;

in vec4 vPosition;
in vec2 vTexCoord;

out vec2 texCoord;

void main(void)
{
    texCoord = vTexCoord;
    gl_Position = vProjection * vModelView * vPosition;
}

