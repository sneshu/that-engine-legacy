#version 450

#include "Data\RenderMode.glsl"
#include "Data\GlobalData.glsl"

layout(input_attachment_index = 0, set = 0, binding = 3) uniform subpassInput u_FramebufferColor;
layout(input_attachment_index = 1, set = 0, binding = 4) uniform subpassInput u_Depth;

layout(location = 0) in vec2 v_UV;

layout(location = 0) out vec4 fragColor;

float GetLinearDepth(float depth)
{
    float near = GlobalData.ScreenSize.z;
    float far = GlobalData.ScreenSize.w;
    float value = (2.0 * near) / (far + near - depth * (far - near));
    return clamp(value, 0.0, 1.0);
}

float GetLinearFogFactor(float depth)
{
    float value = (depth / GlobalData.FogParams.x) / (GlobalData.FogParams.x / GlobalData.FogParams.y);
    return clamp(value, 0.0, 1.0);
}

float GetExpFogFactor(float depth)
{
    return depth;
}

void main()
{
    float linearDepth = GetLinearDepth(subpassLoad(u_Depth).r);
    vec4 SkyColorLinear = pow(GlobalData.SkyColor, vec4(2.2));

    // Depth mode
    if (GlobalData.RenderMode == RENDER_MODE_DEPTH)
    {
        float linearDepthInverted = 1.0 - linearDepth;
        fragColor = vec4(vec3(linearDepthInverted), 1.0);
    }

    // Color mode
    else
    {
        vec4 color = subpassLoad(u_FramebufferColor);
        float fog = GetLinearFogFactor(linearDepth);
        fragColor = mix(color, SkyColorLinear, fog);
    }
}      