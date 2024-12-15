#version 330

// Output of Fragment Shader is Frag Color [Pixel Color]
out vec4 frag_color;


#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_POINT 1
#define LIGHT_TYPE_SPOT 2

struct Light {
    int LightType; // 0 for directional light, 1 for point light, 2 for spot light
    vec3 direction; // Directional Light/spotlight // from src to frag 
    vec3 position; // for point light
    vec3 color; // color of light 
    vec3 attenuation ; // for point/spot light // constant, linear, quadratic
    vec2 cone_angles; // for spot light, inner and out cone angles ;
};

struct Material{
    vec3 ambient; // ambient color of materila 
    vec3 diffuse; 
    vec3 specular;
    float shininess;
};

// Varyings
in Varyings {
    vec4 color;
    vec2 tex_coord;
    vec3 normal;
    vec3 view; // from fragment to camera ; // needed for phong refleciton 
    vec3 fragment_position; // Position of the Fragment in World Space 
} fs_in;

#define MAX_LIGHTS 8
uniform Light lights[MAX_LIGHTS];
uniform int num_lights;

uniform Material material;
uniform vec3 ambient_light; // Ambient Light Intensity ; separated since it is related to enviroment and not light source

uniform sampler2D tex;
uniform vec4 tint;



// Lambert's Cosine Law
float lambertian(vec3 normal, vec3 light_direction){
    return max(0.0,dot(normal, light_direction));
}
// Phong Reflection Model
float phong(vec3 reflected, vec3 view, float shininess){
    return pow(max(0.0, dot(reflected, view)), shininess);
}


vec3 calculateLighting(Light light, vec3 normal, vec3 view, vec3 fragment_pos) {
    vec3 light_direction;
    float attenuation = 1.0; // default attenuation 
    
    // Calculate light direction and attenuation based on light type
    if(light.LightType == LIGHT_TYPE_DIRECTIONAL) {
        light_direction = -normalize(light.direction); // add -ve sign to make from frag to src as light equation need this
    } else {
        vec3 world_to_light = light.position - fragment_pos;
        float distance = length(world_to_light);
        light_direction = normalize(world_to_light);
        
        // Calculate attenuation for point and spot lights
        attenuation = 1.0 / max(dot(light.attenuation, 
                               vec3(1.0, distance, distance * distance)), 0.001); // to avoid division by zero for safety 
        
        // Additional spot light calculations
        if(light.LightType == LIGHT_TYPE_SPOT) {
            float cos_angle = dot(normalize(light.direction), -light_direction);
            float angle = acos(cos_angle);
            attenuation *= smoothstep(light.cone_angles.y, light.cone_angles.x, angle);
        }
    }
    
    // Calculate diffuse
     // Diffuse Component [Diffuse = Kd * Id * Max(0,l.n) ]
   
    vec3 diffuse = material.diffuse * light.color * lambertian(normal, light_direction);
    
    // Specular Component [Specular = Ks * Is * Max(0, (r.v))^alpha]
    vec3 reflected = reflect(-light_direction, normal);
    vec3 specular = material.specular * light.color * 
                    phong(reflected, view, material.shininess);
    
    return (diffuse + specular) * attenuation;
}
void main(){
    
    vec3 initial_color =  vec3(tint * fs_in.color * texture(tex, fs_in.tex_coord));
    
    vec3 ambient = ambient_light * material.ambient;

    vec3 total_light = ambient; // adding ambient light to scene just one time 
    for(int i = 0; i < num_lights; i++) {
        total_light += (calculateLighting(lights[i], fs_in.normal, fs_in.view, fs_in.fragment_position));
    }

    vec3 final_color = initial_color * total_light;
    frag_color = vec4(final_color, 1.0);
}