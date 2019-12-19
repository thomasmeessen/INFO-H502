#version 330 core
layout (location = 0) in vec3 position;
layout(location = 1) in vec3 normals;
layout(location=2) in vec2 texCoo;
out vec3 normal;
out vec2 texCoord;
uniform mat4 MVP;
void main() {
    gl_Position = MVP * vec4(position, 1.0f);
    normal = normals;
    texCoord=texCoo;
}