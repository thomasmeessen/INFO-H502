#version 330 core
layout (location = 0) in vec3 vertex; // <vec2 position, vec2 texCoords>
layout (location = 1) in vec3 aPos;
layout (location = 2) in int a_timer;

out int timer;

void main()
{
    timer = a_timer;
    gl_Position = vec4(aPos + vertex/300,1.0f);
}