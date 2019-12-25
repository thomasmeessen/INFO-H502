#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 9) out;

in int timer[];
in vec3 center[];
out vec4 gs_color;

uniform mat4 view_projection;

vec4 get_explode(vec3 point){
    float explode_dist = 80 - timer[0];
    vec3 dir = point - center[0];
    return vec4(dir * (explode_dist/80.0f + 1.0f) * 1.10f , 0.0);
}


void main() {
    if(timer[0] < 80){
        vec3 barycentre = vec3(gl_in[0].gl_Position) + vec3(gl_in[1].gl_Position)  + vec3(gl_in[2].gl_Position);
        barycentre /= 3;
        int i;
        for (i = 0; i< 3; i++){
            vec4 explode_drift = get_explode( (vec3(gl_in[i].gl_Position) + vec3(gl_in[(i+1)%3].gl_Position) + barycentre)/3);
            if( explode_drift.x >= 0){
                gs_color = vec4 (0.5, 0.5, 0.8, 1.0);
                gl_Position = view_projection * (gl_in[i].gl_Position + explode_drift);
                EmitVertex();
                gl_Position = view_projection * (gl_in[(i+1)%3].gl_Position + explode_drift);
                EmitVertex();
                gs_color = vec4 (0.99, 0.99, 0.99, 1.0);
                gl_Position = view_projection * (vec4(barycentre, 1.0) + explode_drift);
                EmitVertex();
                EndPrimitive();
            } else {

            }
        }
    }else {
        int i;
        for (int i = 0; i< 3; i++){
            gs_color = vec4 (0.5, 0.5, 0.8, 1.0);
            gl_Position = view_projection * gl_in[i].gl_Position;
            EmitVertex();
        }
        EndPrimitive();
    }
}