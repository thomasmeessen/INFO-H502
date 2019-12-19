#version 330 core
in vec3 normal;
in vec2 texCoord;

out vec4 color;

uniform sampler2D texture_diffuse1;

uniform float opacity;
void main(){
    color = texture(texture_diffuse1, texCoord);
}
