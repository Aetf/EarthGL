uniform mat4 vProjection;
uniform mat4 vModelView;

attribute vec4 vPosition;
attribute vec4 vColor;

varying vec4 varyingColor;

void main(void)
{
    varyingColor = vColor;
    gl_Position = vProjection * vModelView * vPosition;
}

