#version 330 core
out vec4 fragColor;

uniform vec3 iResolution;   // viewport resolution (in pixels)
uniform float iTime;        // shader playback time (in seconds)
uniform vec4 iMouse;        // xy: current (if MLB down), zw: click

// Author @patriciogv ( patricio.io ) - 2015
// Title: Nina Warmerdam ( www.behance.net/ninawarmerdam )

#ifdef GL_ES
precision mediump float;
#endif

vec2 tile(vec2 uv, float zoom){
    uv *= zoom;
    return fract(uv);
}

float circle(vec2 uv, float radius){
    vec2 pos = vec2(0.5)-uv;
    radius *= 0.75;
    return 1.-smoothstep(radius-(radius*0.05),radius+(radius*0.05),dot(pos,pos)*3.14);
}

float circlePattern(vec2 uv, float radius) {
    return  circle(uv+vec2(0.,-.5), radius)+
            circle(uv+vec2(0.,.5), radius)+
            circle(uv+vec2(-.5,0.), radius)+
            circle(uv+vec2(.5,0.), radius);
}

void main(){
    vec2 uv = (gl_FragCoord.xy - .5 * iResolution.xy) / iResolution.y;
    vec3 color = vec3(0.0);

    vec2 grid1 = tile(uv,7.);
    grid1 = tile(uv + vec2(cos(iTime),sin(iTime))*0.01,7.);
    color += mix(vec3(0.075,0.114,0.329),vec3(0.973,0.843,0.675),circlePattern(grid1,0.23)-circlePattern(grid1,0.01));

    vec2 grid2 = tile(uv,3.);
    grid2 = tile(uv + vec2(cos(iTime),sin(iTime))*0.02 ,3.);
    color = mix(color, vec3(0.761,0.247,0.102), circlePattern(grid2,0.2)) - circlePattern(grid2,0.05),

    fragColor = vec4(color,1.0);
}