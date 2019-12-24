#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 5) out;

out vec4 gs_color;

uniform mat4 view_projection;

vec3 GetNormal()
{
    vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
    vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
    return normalize(cross(a, b));
}

void main() {
    int i;
    vec3 center = (vec3(gl_in[0].gl_Position ) + vec3(gl_in[1].gl_Position)  + vec3(gl_in[2].gl_Position)) / 3;
    vec3 normal = GetNormal();
    gs_color = vec4 (0.5,0.5,0.8,1.0);
    gl_Position = view_projection * gl_in[0].gl_Position ;
    EmitVertex();
    gl_Position = view_projection * gl_in[1].gl_Position;
    EmitVertex();
    gs_color = vec4 (0.99,0.99,0.99,1.0);
    gl_Position = view_projection * vec4(center  + normal / 300,1.0) ;
    EmitVertex();
    gs_color = vec4 (0.5,0.5,0.8,1.0);
    gl_Position =  view_projection * gl_in[2].gl_Position;
    EmitVertex();
    gl_Position = view_projection * gl_in[0].gl_Position;
    EmitVertex();
    EndPrimitive();
}