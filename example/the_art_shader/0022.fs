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

// Signed distance function for a circle
vec3 sdfCircle(vec2 uv, float r) {
    float x = uv.x;
    float y = uv.y;

    float d = length(vec2(x, y)) - r;

    return d > 0. ? vec3(1.) : vec3(0., 0., 1.);
}

// Anti-aliased circle
float circle(in vec2 uv, in float _radius){
	return 1.-smoothstep(_radius-(_radius*0.01),
                         _radius+(_radius*0.01),
                         dot(uv,uv)*4.0);
}

void main(){
	vec2 uv = gl_FragCoord.xy/iResolution.xy;
	uv -= 0.5; // <-0.5, 0.5>
	//uv.x *= iResolution.x/iResolution.y; // fix aspect ratio
	#if 1
        vec3 color = vec3(circle(uv,0.9));
    #else
		vec3 color = sdfCircle(uv, 0.45); 
	#endif

	gl_FragColor = vec4( color, 1.0 );
}