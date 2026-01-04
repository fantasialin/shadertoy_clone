#version 330 core
out vec4 fragColor;

uniform vec3 iResolution;   // viewport resolution (in pixels)
uniform float iTime;        // shader playback time (in seconds)
uniform vec4 iMouse;        // xy: current (if MLB down), zw: click

// Author @patriciogv - 2015
// http://patriciogonzalezvivo.com

#ifdef GL_OES_standard_derivatives
#extension GL_OES_standard_derivatives : enable
#endif

#ifdef GL_ES
precision mediump float;
#endif

#define PI 3.14159265359
#define TWO_PI 6.28318530718

float shape(vec2 uv, int N){
    uv = uv *2.-1.;
    float a = atan(uv.x,uv.y)+PI;
    float r = TWO_PI/float(N);
    return cos(floor(.5+a/r)*r-a)*length(uv);
}

// Antialiazed Step function
// from http://webstaff.itn.liu.se/~stegu/webglshadertutorial/shadertutorial.html
float aastep(float threshold, float value) {
  #ifdef GL_OES_standard_derivatives
  float afwidth = 0.7 * length(vec2(dFdx(value), dFdy(value)));
  return smoothstep(threshold-afwidth, threshold+afwidth, value);
  #else
  return step(threshold, value);
  #endif
}

void main(){
  vec2 uv = gl_FragCoord.xy/iResolution.xy;
  uv.x *= iResolution.x/iResolution.y;
  vec3 color = vec3(0.0);
  float d = 0.0;

  d = min(shape(uv,3),shape(uv+vec2(0.,0.19),4));

  gl_FragColor = vec4(vec3(1.0-aastep(.2,d)),1.0);
}
