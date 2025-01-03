#version 330 core

// The texture holding the scene pixels
uniform sampler2D tex;

// Read "assets/shaders/fullscreen.vert" to know what "tex_coord" holds;
in vec2 tex_coord;

out vec4 frag_color;

// Vignette is a postprocessing effect that darkens the corners of the screen
// to grab the attention of the viewer towards the center of the screen

void main(){
    //TODO: Modify this shader to apply vignette
    // To apply vignette, divide the scene color
    // by 1 + the squared length of the 2D pixel location the NDC space
    // Hint: remember that the NDC space ranges from -1 to 1
    // while the texture coordinate space ranges from 0 to 1
    // We have the pixel's texture coordinate, how can we compute its location in the NDC space?
    
    
    // calculate pixel's location in NDC space (subtract 1 to make the range -1 to 1)
    vec2 coordNDC = (2.0 * tex_coord) - 1.0;

    // calculate squared length of the pixel's location in NDC space
    float squaredLengthNDC = dot(coordNDC, coordNDC);

    // calculate the vignette value
    float vignetteValue = 1.0 / (1.0 + squaredLengthNDC);

    // apply vignette effect
    frag_color = texture(tex, tex_coord) * vignetteValue;
}