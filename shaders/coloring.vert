#version 430 core

uniform mat4 vProjection;
uniform mat4 vModelView;

in vec4 vPosition;
in vec4 vColor;

out vec4 varyingColor;

void main(void)
{
    varyingColor = vColor;
    gl_Position = vProjection * vModelView * vPosition;
}

