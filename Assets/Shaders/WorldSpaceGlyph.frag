#version 450

#include "Data\GlobalData.glsl"
#include "Data\RenderMode.glsl"
#include "Utils\Utils.glsl"

layout(set = 0, binding = 2) uniform sampler2D u_Texture;

layout(location = 0) in vec2 v_UV;
layout(location = 1) in vec3 v_Normal; 
layout(location = 2) in vec4 v_Color; 

layout(location = 0) out vec4 fragColor;

void main()
{
    // Normals mode
    if (GlobalData.RenderMode == RENDER_MODE_NORMALS)
    {
        float r = RemapNormalChannel(v_Normal.r);
        float g = RemapNormalChannel(v_Normal.g);
        float b = RemapNormalChannel(v_Normal.b);
        fragColor = vec4(r, g, b, 1.0);
    }

    // Triangles mode
    else if (GlobalData.RenderMode == RENDER_MODE_TRIANGLES)
    {
        fragColor = v_Color;
    }

    // Wireframe mode
    else if (GlobalData.RenderMode == RENDER_MODE_WIREFRAME)
    {
        fragColor = vec4(1.0);
    }

    // Color mode
    else
    {
        float alpha = texture(u_Texture, v_UV).r;
        if (alpha < 0.01) discard;
        fragColor = vec4(v_Color.rgb, v_Color.a * alpha);
    }
}