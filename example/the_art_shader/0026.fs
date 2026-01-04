#version 330 core
out vec4 fragColor;

uniform vec3 iResolution;   // viewport resolution (in pixels)
uniform float iTime;        // shader playback time (in seconds)
uniform vec4 iMouse;        // xy: current (if MLB down), zw: click

// Author @kynd - 2016
// Title: Distance field metaball
// http://www.kynd.info

#ifdef GL_ES
precision mediump float;
#endif

float smoothen(float d1, float d2) {
    float k = 1.5;
    return -log(exp(-k * d1) + exp(-k * d2)) / k;
}

void main() {
    vec2 uv = gl_FragCoord.xy/iResolution.xy;
    vec2 p0 = vec2(cos(iTime) * 0.3 + 0.5, 0.5);
    vec2 p1 = vec2(-cos(iTime) * 0.3 + 0.5, 0.5);
    float d = smoothen(distance(uv, p0) * 5.0, distance(uv, p1) * 5.0);
	float ae = 5.0 / iResolution.y;
    vec3 color = vec3(smoothstep(0.8, 0.8+ae, d));
    gl_FragColor = vec4(color, 1.0);
}