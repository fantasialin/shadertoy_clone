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

#define grid_x_shape 1

vec3 sdfSquare(vec2 uv, float size, vec2 offset) {
  float x = uv.x - offset.x;
  float y = uv.y - offset.y;
  float d = max(abs(x), abs(y)) - size;

  return d > 0. ? vec3(1.) : vec3(1., 0., 0.);
}

float box(vec2 uv, vec2 size){
    size = vec2(0.5)-size*0.5;
    vec2 uv_ = smoothstep(size,size+vec2(1e-4),uv);
    uv_ *= smoothstep(size,size+vec2(1e-4),vec2(1.0)-uv);
    return uv_.x*uv_.y;
}

float X_shape(vec2 uv, float width){
    float pct0 = smoothstep(uv.x-width,uv.x,uv.y);
    pct0 *= 1.-smoothstep(uv.x,uv.x+width,uv.y);

    float pct1 = smoothstep(uv.x-width,uv.x,1.0-uv.y);
    pct1 *= 1.-smoothstep(uv.x,uv.x+width,1.0-uv.y);

    return pct0+pct1;
}


void main(void){
    vec2 uv = (gl_FragCoord.xy - .5 * iResolution.xy) / iResolution.y;

    vec2 uv_zoom = fract(uv*20.);

    vec2 offset = vec2(0.0, 0.0);

    vec3 col1 = sdfSquare(uv, 0.2, offset);
    #if grid_x_shape
        vec3 col = vec3(X_shape(uv_zoom,0.03));
    #else
        vec3 col = vec3(box(uv_zoom,vec2(0.9)));
    #endif

    
    col = min(col, col1);

    fragColor = vec4(col,1.0);
}
