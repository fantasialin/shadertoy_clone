#version 330 core
out vec4 fragColor;

uniform vec3 iResolution;   // viewport resolution (in pixels)
uniform float iTime;        // shader playback time (in seconds)
uniform vec4 iMouse;        // xy: current (if MLB down), zw: click

// Author @patriciogv ( patriciogonzalezvivo.com ) - 2015

#ifdef GL_ES
precision mediump float;
#endif

#define enable_pattern_1 1

#define PI 3.14159265358979323846

vec2 rotate2D (vec2 uv, float _angle) {
    uv -= 0.5;
    uv =  mat2(cos(_angle),-sin(_angle),
                sin(_angle),cos(_angle)) * uv;
    uv += 0.5;
    return uv;
}

vec2 tile (vec2 uv, float zoom) {
    uv *= zoom;
    return fract(uv);
}

vec2 rotateTilePattern(vec2 uv){

    //  Scale the coordinate system by 2x2
    uv *= 2.0;

    //  Give each cell an index number
    //  according to its position
    float index = 0.0;
    index += step(1., mod(uv.x,2.0));
    index += step(1., mod(uv.y,2.0))*2.0;

    //      |
    //  2   |   3
    //      |
    //--------------
    //      |
    //  0   |   1
    //      |

    // Make each cell between 0.0 - 1.0
    uv = fract(uv);

    // Rotate each cell according to the index
    if(index == 1.0){
        //  Rotate cell 1 by 90 degrees
        uv = rotate2D(uv,PI*0.5);
    } else if(index == 2.0){
        //  Rotate cell 2 by -90 degrees
        uv = rotate2D(uv,PI*-0.5);
    } else if(index == 3.0){
        //  Rotate cell 3 by 180 degrees
        uv = rotate2D(uv,PI);
    }

    return uv;
}

void main (void) {
    vec2 uv = (gl_FragCoord.xy - .5 * iResolution.xy) / iResolution.y;

    #if enable_pattern_1
        uv = tile(uv,20.0);
        uv = rotateTilePattern(uv);
    #else
        // Make more interesting combinations
        uv = tile(uv,20.0);
        uv = rotate2D(uv,-PI*iTime*0.25);
        uv = rotateTilePattern(uv*2.);
        uv = rotate2D(uv,PI*iTime*0.25);
    #endif

    // step(uv.x,uv.y) just makes a b&w triangles
    // but you can use whatever design you want.
    fragColor = vec4(vec3(step(uv.x,uv.y)),1.0);
}
