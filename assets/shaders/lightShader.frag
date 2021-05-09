R"(//#version 330 core

#ifdef CUGLES
// This one line is all the difference
precision highp float;  // highp required for gradient precision
#endif

// Bit vector for texturing, gradients, and scissoring
uniform int  uType;
// Blur offset for simple kernel blur
uniform vec2 uBlur;

// The texture for sampling
uniform sampler2D uTexture;


//16 is maximum possible rooms in a map
uniform vec3 uRoomLights[16];

uniform vec4 uPlayers[3];

uniform float uLightAngle;

// The output color
out vec4 frag_color;

//pass in character direction, radius, and degree

// The inputs from the vertex shader
in vec2 outPosition;
in vec4 outColor;
in vec2 outTexCoord;

float rangle (vec2 v) {
    float a = atan(v.y/v.x);
    return v.x > 0.? a: a + 3.14159;
}
float dangle (float a, float b) {
    float d = abs(a-b);
    return d > 3.14159 ? d - 3.14159 * 2. : d;
}
float sq (float x) {
    return x * x;
}

float dist (vec2 v1, vec2 v2) {
    float d = sq(v1.x - v2.x) + sq(v1.y - v2.y);
    return sqrt(d);
}

// The stroke+gradient uniform block
layout (std140) uniform uContext
{
    mat3 scMatrix;      // 48
    vec2 scExtent;      //  8
    vec2 scScale;       //  8
    mat3 gdMatrix;      // 48
    vec4 gdInner;       // 16
    vec4 gdOuter;       // 16
    vec2 gdExtent;      //  8
    float gdRadius;     //  4
    float gdFeathr;     //  4
};

/**
 * Returns an interpolation value for a box gradient
 *
 * The value returned is the mix parameter for the inner and
 * outer color.
 *
 * Adapted from nanovg by Mikko Mononen (memon@inside.org)
 *
 * pt:      The (transformed) point to test
 * ext:     The gradient extent
 * radius:  The gradient radius
 * feather: The gradient feather amount
 */
float boxgradient(vec2 pt, vec2 ext, float radius, float feather) {
    vec2 ext2 = ext - vec2(radius,radius);
    vec2 dst = abs(pt) - ext2;
    float m = min(max(dst.x,dst.y),0.0) + length(max(dst,0.0)) - radius;
    return clamp((m + feather*0.5) / feather, 0.0, 1.0);
}

/**
 * Returns an alpha value for scissoring
 *
 * A pixel with value 0 is dropped, while one with value 1 is kept.
 * The scale value sets the 0 to 1 transition (which should be quick).
 *
 * Adapted from nanovg by Mikko Mononen (memon@inside.org)
 *
 * pt:  The point to test
 */
float scissormask(vec2 pt) {
    vec2 sc = (abs((scMatrix * vec3(pt,1.0)).xy) - scExtent);
    sc = vec2(0.5,0.5) - sc * scScale;
    return clamp(sc.x,0.0,1.0) * clamp(sc.y,0.0,1.0);
}

/**
 * Returns the result of a simple kernel blur
 *
 * This function blurs the texture with a simple 9-element
 * kernel.  It is not meant to provide a full-feature
 * Gaussian blur. It is a high performance blur for simple
 * effects (like font blurring).
 *
 * The idea for this function was taken from
 *     https://learnopengl.com/Advanced-OpenGL/Framebuffers
 *
 * coord: The texture coordinate to blur
 */
vec4 blursample(vec2 coord) {
    // Sample offsets
    vec2 off[9] = vec2[](
        vec2(-uBlur.x,  uBlur.y), // top-left
        vec2(    0.0f,  uBlur.y), // top-center
        vec2( uBlur.x,  uBlur.y), // top-right
        vec2(-uBlur.x,     0.0f), // center-left
        vec2(    0.0f,     0.0f), // center-center
        vec2( uBlur.x,     0.0f), // center-right
        vec2(-uBlur.x, -uBlur.y), // bottom-left
        vec2(    0.0f, -uBlur.y), // bottom-center
        vec2( uBlur.x, -uBlur.y)  // bottom-right
    );
    
    // Blur kernel
    float kernel[9] = float[](
        1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0,
        2.0 / 16.0, 4.0 / 16.0, 2.0 / 16.0,
        1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0
    );
    
    // Sample from the texture and average
    vec4 result = vec4(0.0);
    for(int ii = 0; ii < 9; ii++) {
        result += texture(uTexture, coord + off[ii])*kernel[ii];
    }

    return result;
}


float roomshade(vec3 light, vec2 pos) {
    float a = 0.0;
    if (light.z == 0.0f) return a;
    vec2 room = light.xy - pos;
    //sharpness of outer circle edge
    float roomRadius = 200.0;
    float roomOuterRadius = 210.0;
    float rD = dist(light.xy, pos);
    if (rD <= roomRadius) {
        a = 1.0;
    } else {
        a = smoothstep(roomOuterRadius, roomRadius, rD);
    }
    return a;
}

float flashshade(vec4 pal, vec2 pos) {
    float a = 0.0;
    if (pal.z == 0.0f) {
        return a;
    }
    vec2 coor = pal.xy - pos;
    float mangle = rangle(coor);
    //the flashlight angle
    float pangle = pal.w;
    //sharpness of outer circle edge
    float palRadius = 200.0;
    float palOuterRadius = 210.0;
    float d = dist(pal.xy, pos);
    //adjust 0.2 and 0.1 to make edge sharper
    float inAngle = smoothstep(0.5, 0.4, sq(
        dangle(pangle, mangle)
    ));
    if (d <= palRadius) {
        a = inAngle;
    } else {
        a = inAngle*smoothstep(palOuterRadius, palRadius, d);
    }
    return a;

}




/**
 * Performs the main fragment shading.
 * idea for function taken from https://www.shadertoy.com/view/WsySRV
 */
void main(void) {
    vec4 result = texture(uTexture, outTexCoord);
    if (result.a == 0.0) {
        frag_color = result;

    } else {
        float a = 1.0;
        
        for (int i = 0; i < 3; i++) {
            float flashcolor = flashshade(uPlayers[i], outPosition);
            if (flashcolor > 0.0) {
                a = max((a - flashcolor),0.0);
            }
            
        }

        for (int i = 0; i < 16; i++) {
            float roomcolor = roomshade(uRoomLights[i], outPosition);
            if (roomcolor > 0.0) {
                a = max((a - roomcolor),0.0);
            }
        }

        frag_color = vec4(result.rgb, a);
    }

}
)"
