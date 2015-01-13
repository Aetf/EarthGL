uniform mat4 vProjection;
uniform mat4 vModelView;

attribute vec4 vPosition;
attribute vec2 vTexCoord;

varying vec2 texCoord;

void main(void)
{
    texCoord = vTexCoord;
    gl_Position = vProjection * vModelView * vPosition;
}

