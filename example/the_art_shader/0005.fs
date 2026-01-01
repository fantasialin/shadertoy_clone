#version 330 core
out vec4 fragColor;

uniform vec3 iResolution;   // viewport resolution (in pixels)
uniform float iTime;        // shader playback time (in seconds)
uniform vec4 iMouse;        // xy: current (if MLB down), zw: click

// Author @patriciogv - 2015 - patriciogonzalezvivo.com

#ifdef GL_ES
precision mediump float;
#endif

vec2 skew (vec2 uv) {
    vec2 r = vec2(0.0);
    r.x = 1.1547*uv.x;
    r.y = uv.y+0.5*r.x;
    return r;
}

vec3 simplexGrid (vec2 uv) {
    vec3 xyz = vec3(0.0);

    vec2 p = fract(skew(uv));
    if (p.x > p.y) {
        xyz.xy = 1.0-vec2(p.x,p.y-p.x);
        xyz.z = p.y;
    } else {
        xyz.yz = 1.0-vec2(p.x-p.y,p.y);
        xyz.x = p.x;
    }

    return fract(xyz);
}

void main() {
    vec2 uv = gl_FragCoord.xy/iResolution.xy;
    vec3 color = vec3(0.0);

    // Scale the space to see the grid
    uv *= 10.;

    // Show the 2D grid
    color.rg = fract(uv);

    // Skew the 2D grid
    // color.rg = fract(skew(uv));

    // Subdivide the grid into to equilateral triangles
    // color = simplexGrid(uv);

    fragColor = vec4(color,1.0);
}