#version 330 core
out vec4 fragColor;

uniform vec3 iResolution;   // viewport resolution (in pixels)
uniform float iTime;        // shader playback time (in seconds)
uniform vec4 iMouse;        // xy: current (if MLB down), zw: click

// Author @kynd - 2016
// Title: Distance Field Shapes 
// http://www.kynd.info

#ifdef GL_ES
precision mediump float;
#endif

float smoothedge(float v) {
    return smoothstep(0.0, 1.0 / iResolution.x, v);
}

float circle(vec2 p, float radius) {
  return length(p) - radius;
}

float rect(vec2 p, vec2 size) {  
  vec2 d = abs(p) - size;
  return min(max(d.x, d.y), 0.0) + length(max(d,0.0));
}

float roundRect(vec2 p, vec2 size, float radius) {
  vec2 d = abs(p) - size;
  return min(max(d.x, d.y), 0.0) + length(max(d,0.0))- radius;
}

float ring(vec2 p, float radius, float width) {
  return abs(length(p) - radius * 0.5) - width;
}

float hexagon(vec2 p, float radius) {
    vec2 q = abs(p);
    return max(abs(q.y), q.x * 0.866025 + q.y * 0.5) - radius;
}

float triangle(vec2 p, float size) {
    vec2 q = abs(p);
    return max(q.x * 0.866025 + p.y * 0.5, -p.y * 0.5) - size * 0.5;
}

float ellipse(vec2 p, vec2 r, float s) {
    return (length(p / r) - s);
}

float capsule(vec2 p, vec2 a, vec2 b, float r) {
    vec2 pa = p - a, ba = b - a;
    float h = clamp( dot(pa,ba)/dot(ba,ba), 0.0, 1.0 );
    return length( pa - ba*h ) - r;
}

//http://thndl.com/square-shaped-shaders.html
float polygon(vec2 p, int vertices, float size) {
    float a = atan(p.x, p.y) + 0.2;
    float b = 6.28319 / float(vertices);
    return cos(floor(0.5 + a / b) * b - a) * length(p) - size;
}

void main() {
    vec2 uv = gl_FragCoord.xy / iResolution.xy;

    float d = circle(uv - vec2(0.2), 0.1);
    d = min(d, rect(uv - vec2(0.5, 0.2), vec2(0.1, 0.08)));
    d = min(d, roundRect(uv - vec2(0.8, 0.2), vec2(0.08, 0.06), 0.02));
    d = min(d, ring(uv - vec2(0.2, 0.5), 0.18, 0.02));
    d = min(d, hexagon(uv - vec2(0.5, 0.5), 0.1));
    d = min(d, triangle(uv - vec2(0.8, 0.5), 0.1));
    d = min(d, ellipse(uv - vec2(0.2, 0.8), vec2(0.9, 1.2), 0.1));
    d = min(d, capsule(uv - vec2(0.5, 0.8), vec2(-0.05, -0.05), vec2(0.05, 0.05), 0.05));
    d = min(d, polygon(uv - vec2(0.8, 0.8), 5, 0.1));
    vec3 color = vec3(smoothedge(d));
    fragColor = vec4(color, 1.0);
}