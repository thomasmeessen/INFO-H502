#version 330 core
layout (location = 0) in vec3 vertex; // <vec2 position, vec2 texCoords>
layout (location = 1) in vec3 aPos;
layout (location = 2) in vec4 aCol;

out vec4 ParticleColor;
uniform mat4 mvp;
void main()
{
    ParticleColor = aCol;
    vec4 pos = mvp*vec4(aPos + vertex/100,1.0f);
    gl_Position = pos;
}