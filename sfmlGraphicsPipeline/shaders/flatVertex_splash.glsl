# version 400 // GLSL version, fit with OpenGL version

uniform mat4 projMat, viewMat, modelMat;
in vec3 vPosition;
in vec4 vColor;
in vec3 vNormal;
out vec4 color;
out vec3 normal;

void main (){
// Transform coordinates from local space to clipped space
gl_Position = projMat * viewMat * modelMat * vec4 (vPosition, 1);
color = vColor;
normal = vNormal;
}