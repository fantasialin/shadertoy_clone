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

// Based on https://www.shadertoy.com/view/4sSSzG
float triangleDF(vec2 uv,
                vec2 p0, vec2 p1, vec2 p2){
  vec3 e0, e1, e2;

  e0.xy = normalize(p1 - p0).yx * vec2(+1.0, -1.0);
  e1.xy = normalize(p2 - p1).yx * vec2(+1.0, -1.0);
  e2.xy = normalize(p0 - p2).yx * vec2(+1.0, -1.0);

  e0.z = dot(e0.xy, p0);
  e1.z = dot(e1.xy, p1);
  e2.z = dot(e2.xy, p2);

  float a = max(0.0, dot(e0.xy, uv) - e0.z);
  float b = max(0.0, dot(e1.xy, uv) - e1.z);
  float c = max(0.0, dot(e2.xy, uv) - e2.z);

  return length(vec3(a, b, c)*2.0);
}

void main(){
  vec2 uv = gl_FragCoord.xy/iResolution.xy;
  vec3 color = vec3(0.0);

  // Distance Field in 3 channels
  float df = triangleDF(uv,
                       vec2(0.40,0.45),
                       vec2(0.60,0.45),
                       vec2(0.5,0.60));
  color = vec3(df);

  // Make a shape of it
  float size = fract(iTime*0.2);
  float border = 0.025;
  color.rb += smoothstep(size+border,size+1e-7,df)-
              smoothstep(size+0.001,size+1e-7,df);

  gl_FragColor = vec4(color,1.0);
}
