#version 330 core
layout (location = 0) in vec3 position;

uniform mat4 LightSpaceModel;

void main() {
    gl_Position = LightSpaceModel * vec4(position, 1.0f);

}