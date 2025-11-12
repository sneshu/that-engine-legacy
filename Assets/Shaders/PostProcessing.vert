#version 450

#include "Data\GlobalData.glsl"

layout(location = 0) out vec2 v_UV;

vec2 positions[6] = vec2[]( 
    vec2(-1.0, -1.0), 
    vec2(-1.0,  1.0), 
    vec2( 1.0,  1.0), 
    vec2( 1.0,  1.0), 
    vec2( 1.0, -1.0), 
    vec2(-1.0, -1.0)
);

vec2 uvs[6] = vec2[]( 
    vec2(0.0, 0.0), 
    vec2(0.0, 1.0), 
    vec2(1.0, 1.0), 
    vec2(1.0, 1.0), 
    vec2(1.0, 0.0),
    vec2(0.0, 0.0)
);

void main()
{
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
    v_UV = uvs[gl_VertexIndex];
}      