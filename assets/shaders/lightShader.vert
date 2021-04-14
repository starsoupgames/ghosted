R"(//#version 330 core

// Positions
in vec4 aPosition;
out vec2 outPosition;

// Colors
in  vec4 aColor;
out vec4 outColor;

// Texture coordinates
in  vec2 aTexCoord;
out vec2 outTexCoord;

// Matrices
uniform mat4 uPerspective;

// Transform and pass through                                                   
void main(void) {
    gl_Position = uPerspective*aPosition;
    //gl_Position = vec4(0.5 * aPosition.x, 0.5 * aPosition.y, aPosition.z, 1.0);
    outPosition = aPosition.xy; // Need untransformed for scissor
    //outPosition = vec2(0.0, 0.0);
    outColor = aColor;
    outTexCoord = aTexCoord;
}
)"