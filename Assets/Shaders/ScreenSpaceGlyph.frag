#version 450

#include "Data\GlobalData.glsl"

layout(set = 0, binding = 2) uniform sampler2D u_Texture;

layout(location = 0) in vec2 v_UV;
layout(location = 1) in vec3 v_Normal; 
layout(location = 2) in vec4 v_Color; 

layout(location = 0) out vec4 fragColor;

void main()
{
    float alpha = texture(u_Texture, v_UV).r;

    if (alpha < 0.01) discard;

    fragColor = vec4(v_Color.rgb, v_Color.a * alpha);
}