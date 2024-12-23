#version 330 core

uniform sampler2D tex;
in vec2 tex_coord;
out vec4 frag_color;

// Adjust these values to change the effect intensity
#define VIGNETTE_STRENGTH 0.3
#define SEPIA_INTENSITY 0.3
#define GRAIN_AMOUNT 0.09

float random(vec2 coord) {
    return fract(sin(dot(coord, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {
    // Get base color
    vec4 color = texture(tex, tex_coord);
    
    // Add vignette (darker corners)
    vec2 center_dist = tex_coord - 0.5;
    float vignette = 1.0 - dot(center_dist, center_dist) * VIGNETTE_STRENGTH;
    color.rgb *= vignette;
    
    // Convert to sepia tones
    vec3 sepia = vec3(
        dot(color.rgb, vec3(0.393, 0.769, 0.189)),
        dot(color.rgb, vec3(0.349, 0.686, 0.168)),
        dot(color.rgb, vec3(0.272, 0.534, 0.131))
    );
    color.rgb = mix(color.rgb, sepia, SEPIA_INTENSITY);
    
    // Add some static grain
    float grain = random(tex_coord) * GRAIN_AMOUNT;
    color.rgb += grain;
    
    frag_color = color;
}