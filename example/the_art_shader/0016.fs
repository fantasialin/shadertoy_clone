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

float rect(in vec2 uv, in vec2 size){
	size = 0.25-size*0.25;
    vec2 uv_ = smoothstep(size,size+size*vec2(0.002),uv*(1.0-uv));
	return uv_.x*uv_.y;
}

void main(){
	vec2 uv = gl_FragCoord.xy/iResolution.xy;

    uv = fract(uv*10.0);//10x10 grid

	vec3 color = vec3( rect(uv, vec2(0.9) ) );

	fragColor = vec4(color,1.0);
}
