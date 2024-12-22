#version 330

uniform sampler2D tex; // The texture holding the scene pixels
uniform float u_time; // The current time (for shake)
uniform float u_shakeIntensity; // The intensity of the shake (for shake)

in vec2 tex_coord; // Texture coordinate

out vec4 frag_color; // Output color of the pixel

void main() {
    // Calculate NDC Position for both effects
    vec2 ndcPosition = tex_coord * 2.0 - 1.0;
    
    // Shake Effect
    float shakeX = sin(u_time * 15.0) * u_shakeIntensity/2.0; // Shake on the X-axis
    float shakeY = cos(u_time * 17.0) * u_shakeIntensity/2.0; // Shake on the Y-axis
    vec2 shakeOffset = vec2(shakeX, shakeY);
    
    // Get color from the scene with shake effect applied
    vec3 sceneColor = texture(tex, tex_coord + shakeOffset).rgb;
    
    // Vignette Effect
    // Compute the squared distance of the pixel from the center in NDC space for vignette
    float distanceSquared = dot(ndcPosition, ndcPosition);
    
    // Apply vignette by dividing the scene color by 1 + the squared distance
    vec3 vignetteColor = sceneColor / (1.0 + distanceSquared * 1.0);

    // Output the final color with both shake and vignette applied
    frag_color = vec4(vignetteColor, 1.0);
}