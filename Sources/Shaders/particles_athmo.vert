#version 330 core
layout (location = 0) in vec3 vertex; // <vec2 position, vec2 texCoords>
layout (location = 1) in vec3 aPos;



void main()
{
    vec4 pos = vec4(aPos + vertex/200,1.0f);
    gl_Position = pos;
}