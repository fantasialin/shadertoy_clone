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

#define PI 3.14159265359

mat2 rotate2d(float _angle){
    return mat2(cos(_angle),-sin(_angle),
                sin(_angle),cos(_angle));
}

mat2 scale(vec2 _scale){
    return mat2(_scale.x,0.0,
                0.0,_scale.y);
}

float box(in vec2 st, in vec2 size){
    size = vec2(0.5) - size*0.5;
    vec2 uv = smoothstep(size,
                        size+vec2(0.001),
                        st);
    uv *= smoothstep(size,
                    size+vec2(0.001),
                    vec2(1.0)-st);
    return uv.x*uv.y;
}

float cross(in vec2 uv, float size){
    return  box(uv, vec2(size,size/4.)) +
            box(uv, vec2(size/4.,size));
}

#define static_cross 1

void main(){
    vec2 uv = gl_FragCoord.xy/iResolution.xy;
    vec3 color = vec3(0.0);

    #if static_cross
        #if 1
            #if 1
                color = vec3( cross(uv,0.4) );//locate in center of canvas
            #else //rotate cross
                // move space from the center to the vec2(0.0)
                uv -= vec2(0.5);
                // rotate the space
                uv = rotate2d( sin(iTime)*PI ) * uv;
                // move it back to the original place
                uv += vec2(0.5);

                // Show the coordinates of the space on the background
                color = vec3(uv.x,uv.y,0.0);

                // Add the shape on the foreground
                color += vec3(cross(uv,0.4));
            #endif
        #else //scale cross object
            uv -= vec2(0.5);
            uv = scale( vec2(sin(iTime)+1.0) ) * uv;
            uv += vec2(0.5);

            // Show the coordinates of the space on the background
            color = vec3(uv.x,uv.y,0.0);

            // Add the shape on the foreground
            color += vec3(cross(uv,0.2));
        #endif
    #else
        #if 1
            // To move the cross we move the space
            vec2 translate = vec2(cos(iTime),sin(iTime));
            uv += translate*0.35;
            
            // Show the coordinates of the space on the background
            color = vec3(uv.x,uv.y,0.0);

            // Add the shape on the foreground
            color += vec3(cross(uv,0.25));
        #else
            // move space from the center to the vec2(0.0)
            uv -= vec2(0.5);
            // rotate the space
            uv = rotate2d( sin(iTime*0.5)*PI ) * uv;
            // move it back to the original place
            uv += vec2(0.5);

            // To move the cross we move the space
            vec2 translate = vec2(cos(iTime),sin(iTime));
            uv += translate*0.35;

            // Show the coordinates of the space on the background
            color = vec3(uv.x,uv.y,0.0);

            // Add the shape on the foreground
            color += vec3(cross(uv,0.25));
        #endif
    #endif

    gl_FragColor = vec4(color,1.0);
}
