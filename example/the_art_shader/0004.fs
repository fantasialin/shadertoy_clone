#version 330 core
out vec4 fragColor;

uniform vec3 iResolution;   // viewport resolution (in pixels)
uniform float iTime;        // shader playback time (in seconds)
uniform vec4 iMouse;        // xy: current (if MLB down), zw: click

#ifdef GL_ES
precision mediump float;
#endif

// Copyright (c) Patricio Gonzalez Vivo, 2015 - http://patriciogonzalezvivo.com/
// I am the sole copyright owner of this Work.
//
// You cannot host, display, distribute or share this Work in any form,
// including physical and digital. You cannot use this Work in any
// commercial or non-commercial product, website or project. You cannot
// sell this Work and you cannot mint an NFTs of it.
// I share this Work for educational purposes, and you can link to it,
// through an URL, proper attribution and unmodified screenshot, as part
// of your educational material. If these conditions are too restrictive
// please contact me and we'll definitely work it out.

#define PI 3.14159265358979323846

vec2 rotate2D(vec2 _uv, float _angle){
    _uv -= 0.5;
    _uv =  mat2(cos(_angle),-sin(_angle),
      sin(_angle),cos(_angle)) * _uv;
    _uv += 0.5;
    return _uv;
}

vec2 tile(vec2 _uv, float _zoom){
    _uv *= _zoom;
    return fract(_uv);
}

float box(vec2 _uv, vec2 _size, float _smoothEdges){
    _size = vec2(0.5)-_size*0.5;
    vec2 aa = vec2(_smoothEdges*0.5);
    vec2 uv = smoothstep(_size,_size+aa,_uv);
    uv *= smoothstep(_size,_size+aa,vec2(1.0)-_uv);
    return uv.x*uv.y;
}

vec2 offset(vec2 _uv, vec2 _offset){
    vec2 uv;

    if(_uv.x>0.5){
        uv.x = _uv.x - 0.5;
    } else {
        uv.x = _uv.x + 0.5;
    }

    if(_uv.y>0.5){
        uv.y = _uv.y - 0.5;
    } else {
        uv.y = _uv.y + 0.5;
    }

    return uv;
}

void main(void){
    vec2 uv = gl_FragCoord.xy/iResolution.xy;
    uv.y *= iResolution.y/iResolution.x;

    uv = tile(uv,10.);

    vec2 offsetSt = offset(uv,vec2(0.5));

    uv = rotate2D(uv,PI*0.25);

    vec3 color = vec3( box(offsetSt,vec2(0.95),0.01) - box(uv,vec2(0.3),0.01) + 2.*box(uv,vec2(0.2),0.01) );

    fragColor = vec4(color,1.0);
}