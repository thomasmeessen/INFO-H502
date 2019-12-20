#version 330 core
layout (location = 0) in vec3 position;
layout(location = 1) in vec3 aNormals;
layout(location=2) in vec2 texCoo;
layout (location=3) in vec3 aTangent;
layout (location=4) in vec3 aBitangent;

out mat3 TBN;
out vec2 texCoord;
out vec3 fragPos;
out vec4 FragPosLightSpace;
// Proximity of a light source
out vec3 closest_light;


uniform mat4 MVP;
uniform mat4 Model;
uniform mat4 lightSpace;
uniform vec3[50] light_part_list;

void main() {
    gl_Position = MVP * vec4(position, 1.0f);
    vec3 N = normalize( vec3(Model * vec4(aNormals,0.0)));
    vec3 T = normalize(vec3(Model * vec4( aTangent,0.0)));
    vec3 B = normalize(vec3(Model * vec4(aBitangent, 0.0)));
    TBN = mat3(T, B, N);
    fragPos =  vec3(Model * vec4(position, 1.0));
    texCoord=texCoo;
    FragPosLightSpace = lightSpace * vec4(fragPos, 1.0);

    closest_light = vec3(0.0,0.0,0.0);
    for(int i = 0; i < 50 ; i++){
        vec3 model_space_vertex_pos = vec3(Model *  vec4(position, 1.0f));
        float distance = length(model_space_vertex_pos - light_part_list[i]);
        if (distance < 0.5){
            closest_light =  light_part_list[i];
        }
    }

}