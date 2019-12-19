#version 330 core
in vec3 myColor;
out vec3 color;
uniform float opacity;
void main(){
    color = myColor;
}
