#version 430 core

uniform mat4 vProjection;
uniform mat4 vModelView;
uniform mat3 vNormalMatrix;
uniform vec3 vLightPosition;

in vec4 vPosition;
in vec3 vNormal;
in vec2 vTexCoord;

out vec3 normal;
out vec3 lightDir;
out vec3 viewerDir;
out vec2 texCoord;

void main(void)
{
    vec4 eyeVertex = vModelView * vPosition;
    eyeVertex /= eyeVertex.w;

    normal = vNormalMatrix * vNormal;
    lightDir = vLightPosition - eyeVertex.xyz;
    viewerDir = - eyeVertex.xyz;

    texCoord = vTexCoord;

    gl_Position = vProjection * vModelView * vPosition;
}

