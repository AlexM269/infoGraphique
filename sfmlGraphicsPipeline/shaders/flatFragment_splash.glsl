# version 400 // GLSL version, fit with OpenGL version

in vec4 color;
in vec3 normal;
out vec4 fragmentColor;
void main (){
fragmentColor = color;
}