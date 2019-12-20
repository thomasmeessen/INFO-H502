#version 330 core
layout (location = 0) in vec3 vertex; // <vec2 position, vec2 texCoords>
layout (location = 1) in vec3 aPos;

uniform mat4 mvp;

void main()
{
    vec4 pos = mvp*vec4(aPos + vertex/140,1.0f);
    gl_Position = pos;
}