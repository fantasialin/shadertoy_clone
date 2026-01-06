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

void main(){
    vec2 uv = gl_FragCoord.xy/iResolution.xy;
    vec3 color = vec3(0.0);

    vec2 pos = vec2(0.5)-uv;

    float r = length(pos)*2.0;
    float a = atan(pos.y,pos.x);

    float f = cos(a*3.);
    // f = abs(cos(a*3.));
    // f = abs(cos(a*2.5))*.5+.3;
    // f = abs(cos(a*12.)*sin(a*3.))*.8+.1;
    // f = smoothstep(-.5,1., cos(a*10.))*0.2+0.5;

    color = vec3( 1.-smoothstep(f,f+0.02,r) );

    fragColor = vec4(color, 1.0);
}