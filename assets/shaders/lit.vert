#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 tex_coord;
layout(location = 3) in vec3 normal;

out Varyings {
    vec4 color;
    vec2 tex_coord;
    vec3 normal; 
    vec3 view ; // Vector from the Fragment to the Camera 
    vec3 fragment_position; // Position of the Fragment in World Space 
} vs_out;

// Uniforms
uniform mat4 M; // model matrix 
uniform mat4 VP; // view projection 
uniform mat4 M_IT; // model inverse transpose for normals 
uniform vec3 camera_position; // camera position to get the view vector

// vertex shader for materials that iteract with light sources 
void main(){
    // since will enter camera_position as a uniform related to the world ; i want to calc. 
    // the view vector in the vertex shader from framgent to camera both in world space
    vec3 position_world = (M * vec4(position , 1.0f)).xyz; 
    gl_Position = VP * vec4(position_world, 1.0); // transform the position to clip space
    // color 
    vs_out.color = color;
    // texture coord
    vs_out.tex_coord = tex_coord;
    vs_out.normal = normalize(M_IT * vec4(normal,1.0)).xyz; // transform the normal to world space
    vs_out.view = camera_position - position_world;
    vs_out.fragment_position = position_world;

}