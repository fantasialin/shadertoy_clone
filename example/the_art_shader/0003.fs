#version 330 core
out vec4 fragColor;

uniform vec3 iResolution;   // viewport resolution (in pixels)
uniform float iTime;        // shader playback time (in seconds)
uniform vec4 iMouse;        // xy: current (if MLB down), zw: click

#ifdef GL_ES
precision mediump float;
#endif

void main() {
    vec2 uv = gl_FragCoord.xy/iResolution.xy;
    uv.x *= iResolution.x/iResolution.y;

    vec3 color = vec3(0.);
    color = vec3(uv.x,uv.y,abs(sin(iTime)));

    fragColor = vec4(color,1.0);
}