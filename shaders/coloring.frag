#version 430 core

in vec4 varyingColor;

out vec4 fragColor;

void main(void)
{
//    fragColor = vec4(0.0, 0.0, 1.0, 1.0);
    fragColor = varyingColor;
}

