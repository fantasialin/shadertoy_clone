#version 330 core
out vec4 fragColor;

uniform vec3 iResolution;   // viewport resolution (in pixels)
uniform float iTime;        // shader playback time (in seconds)
uniform vec4 iMouse;        // xy: current (if MLB down), zw: click

// Author @patriciogv - 2015
// http://patriciogonzalezvivo.com

#ifdef GL_ES
precision mediump float;
#endif

#define bottom_left 1

void main(){
    vec2 uv = gl_FragCoord.xy/iResolution.xy;
    vec3 color = vec3(0.0);

    //uv = fract(uv*10);
    float pct;
    #if bottom_left
        // bottom-left
        vec2 bl = step(vec2(0.1),uv);
        pct = bl.x * bl.y;
    #else
        // top-right
        vec2 tr = step(vec2(0.1),1.0-uv);
        pct = tr.x * tr.y;
    #endif

    color = vec3(pct);

    fragColor = vec4(color,1.0);
}