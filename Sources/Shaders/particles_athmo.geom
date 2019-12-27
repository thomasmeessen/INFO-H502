#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 9) out;

in int timer[];
in float scale[];
in vec3 center[];
out vec4 gs_color;

uniform mat4 view_projection;

void main() {
    if(timer[0] < 80){
        vec3 barycentre = vec3(gl_in[0].gl_Position) + vec3(gl_in[1].gl_Position)  + vec3(gl_in[2].gl_Position);
        barycentre /= 3;
        int i;
        float explode_time = 80 - timer[0];
        for (i = 0; i< 3; i++)
        {
            vec3 shard_barycentre = ( vec3(gl_in[i].gl_Position) + vec3(gl_in[(i+1)%3].gl_Position)  + vec3(barycentre))/3.0f;
            vec3 shard_displacement = (shard_barycentre - center[0]) *scale[0] * 8.0f * explode_time;
            if(shard_displacement.x < -0.001){
                shard_displacement.x *= - 0.7;
            }
            gs_color = vec4 (0.5, 0.5, 0.8, 1.0);
            vec3 p1 = vec3(gl_in[i].gl_Position) + shard_displacement;
            gl_Position = view_projection * vec4(p1,1.0);
            EmitVertex();
            vec3 p2 = vec3(gl_in[(i+1)%3].gl_Position) + shard_displacement;
            gl_Position = view_projection * vec4(p2,1.0);
            EmitVertex();
            gs_color = vec4 (0.99, 0.99, 0.99, 1.0);
            vec3 p3 = barycentre + shard_displacement;
            gl_Position = view_projection * vec4(p3,1.0);
            EmitVertex();
            EndPrimitive();
        }
    } else {
        int i;
        for (int i = 0; i< 3; i++){
            gs_color = vec4 (0.5, 0.5, 0.8, 1.0);
            gl_Position = view_projection * gl_in[i].gl_Position;
            EmitVertex();
        }
        EndPrimitive();
    }
}