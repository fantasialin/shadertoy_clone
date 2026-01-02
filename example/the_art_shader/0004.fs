#version 330 core
out vec4 fragColor;

uniform vec3 iResolution;   // viewport resolution (in pixels)
uniform float iTime;        // shader playback time (in seconds)
uniform vec4 iMouse;        // xy: current (if MLB down), zw: click

#ifdef GL_ES
precision mediump float;
#endif

#define PI 3.14159265358979323846

#ifdef GL_ES
precision mediump float;
#endif

uniform vec2 u_resolution;
uniform vec2 u_mouse;
uniform float u_time;

#define DRAW_LINE 1

// Plot a line on Y using a value between 0.0-1.0
float plot_line(vec2 uv) {    
    return smoothstep(0.02, 0.0, abs(uv.y - uv.x));
}

float plot_curve(vec2 uv, float pct) { 
    return smoothstep( pct-0.02, pct, uv.y) -
          smoothstep( pct, pct+0.02, uv.y);
}

void main() {
	vec2 uv = gl_FragCoord.xy/iResolution.xy;

    #if DRAW_LINE
    float y = uv.x;
    vec3 col = vec3(y);
    float pct = plot_line(uv);
    #else
    float y = pow(uv.x,5.0);
    vec3 col = vec3(y);
    float pct = plot_curve(uv,y);
    #endif
    vec3 color = mix(col, vec3(0, 1, 0), pct);

	fragColor = vec4(color,1.0);
}
