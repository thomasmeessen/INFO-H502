#version 330 core

in vec4 gs_color;

out vec4 color;


void main()
{
    //color = vec4 (0.5,0.5,0.8,1.0);
    color = gs_color;
}