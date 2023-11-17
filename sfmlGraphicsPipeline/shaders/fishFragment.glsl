#version 400  // GLSL version, fit with OpenGL version

in vec4 surfel_color;

uniform float time;

out vec4 fragmentColor;

void main()
{
    fragmentColor = surfel_color;
}
