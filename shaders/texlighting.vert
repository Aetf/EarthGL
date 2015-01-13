uniform mat4 vProjection;
uniform mat4 vModelView;
uniform mat3 vNormalMatrix;
uniform vec3 vLightPosition;

attribute vec4 vPosition;
attribute vec3 vNormal;
attribute vec2 vTexCoord;

varying vec3 normal;
varying vec3 lightDir;
varying vec3 viewerDir;
varying vec2 texCoord;

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

