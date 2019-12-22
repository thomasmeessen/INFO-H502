#version 330 core
in vec2 texCoord;
in vec3 fragPos;
in mat3 TBN;
in vec4 FragPosLightSpace;
// Weather effect
in vec3 closest_light;

out vec4 color;


uniform vec3 lightPos;
uniform float ambientStrength;

uniform sampler2D depthMap;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D normal_map1;


uniform vec3 viewPos;


float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(depthMap, projCoords.xy).r;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float bias = 0.005; // Shadow acne
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    return shadow;
}

vec4 thruster_diffuse_color(vec3 pos, vec3 normal){
    vec3 purple_thruster_pos = vec3(-0.159,0,0);
    vec3 yellow_thruster_pos = vec3(-0.28,0,0);
    float purple_light_dist = length(purple_thruster_pos - pos);
    vec4 purple_contribution = vec4(0);
    float purple_range = 0.08;
    if (purple_light_dist < purple_range){
        vec3 purple_light_dir = normalize(purple_thruster_pos - pos);
        float purple_diffuseStrength = (min(max(dot(purple_light_dir, normal ),0.5), 0.0) + 0.5);
        vec4 purple_color = vec4(128.0f/255.0f,0.0f,107.0/255.0f,1.0);
        purple_contribution = purple_diffuseStrength * purple_color * (1- purple_light_dist/purple_range);
    }

    float yellow_light_dist = length(yellow_thruster_pos - pos);
    vec4 yello_contrib = vec4(0);
    float yellow_range = 0.12;
    if (yellow_light_dist < yellow_range){
        vec3 yellow_light_dir = normalize(yellow_thruster_pos - pos);
        float yellow_diffuseStrength = (min(max(dot(yellow_light_dir, normal ),0.5), 0.0) + 0.5);
        vec4 yellow = vec4(128.0f/255.0f,0.0f,107.0/255.0f,1.0)*0.2 + vec4(1.0f,1.0f,0,1.0) *0.8;
        yello_contrib = yellow_diffuseStrength * yellow * (1- yellow_light_dist/yellow_range);
    }

    return yello_contrib + purple_contribution;
}


void main(){
    // Normal map
    vec3 normal = texture(normal_map1, texCoord).rgb;
    // Compensate for storage being in the [0,1] range
    normal = normalize(normal * 2.0 - 1.0);
    // Transform the normal from triangle system to world coordinate
    normal = normalize(TBN * normal);
    // Sample Base color
    vec4 objectColor = texture(texture_diffuse1, texCoord);
    // Sample Metallic texture
    vec4 specColor = texture(texture_specular1, texCoord);

    // DIFFUSE
    // Cosmetic adjustment for diffusive importance
    float diffStrength = 0.4f;
    // direction of the main light source
    vec3 lightDir = normalize(lightPos - fragPos);
    // Projection of pixel normal on the light direction and cosmetic adjustment
    float diffuseStrength = max(dot(lightDir, normal ), 0.0) * diffStrength;

    // SPECULAR
    // direction of the camera
    vec3 viewDir = normalize(viewPos - fragPos);
    // Direction of the ideal reflection of light on the pixel
    vec3 reflectDir = reflect(-normalize(lightDir), normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 48)  ;

    // SHADOW
    float shadow = ShadowCalculation(FragPosLightSpace);


    // ambient + diffuse (main)
    color = (ambientStrength + diffuseStrength * (1.0 - shadow)) * objectColor ;
    // specular (main)
    color += spec * (1.0-shadow) * specColor;
    // Thruster effect
    vec4 thruster_effect = thruster_diffuse_color(fragPos, normal);
    if (length(thruster_effect) > 0){
        color += thruster_effect;
    }

 }
