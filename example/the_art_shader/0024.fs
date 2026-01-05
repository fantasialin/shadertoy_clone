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

float plot (float y, float pct){
  return  smoothstep( pct-0.01, pct, y) -
          smoothstep( pct, pct+0.01, y);
}

void main(){
    vec2 uv = gl_FragCoord.xy/iResolution.xy;
    uv.y *= iResolution.y/iResolution.x;
    vec3 color = vec3(.0);
    float pct = .0;

    float shape = 1.0-distance(uv*vec2(1.,2.)-vec2(0.,.5),vec2(.5));

    pct = shape;
    pct = min(pct, distance(uv,vec2(0.5,0.76))*10.);
    pct = min(pct, distance(uv,vec2(0.36,0.71))*5.);
    pct = min(pct, distance(uv,vec2(0.64,0.71))*5.);
    pct = min(pct, distance(uv,vec2(0.36,0.20))*4.*pow(1.-uv.y,shape*1.1));
    pct = min(pct, distance(uv,vec2(0.64,0.20))*4.*pow(1.-uv.y,shape*1.1));

    color = vec3(pct);

    color += vec3(1.,1.,.0)*plot(pct,0.5+smoothstep(-1.,2.,sin(iTime))*.1);


    fragColor = vec4( color, 1.0 );
}
