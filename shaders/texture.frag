#version 430 core

uniform sampler2D tex;

in vec2 texCoord;

out vec4 fragColor;

void main(void)
{
    gl_FragColor = texture(tex, texCoord);
}

