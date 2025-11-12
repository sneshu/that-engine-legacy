#version 450 core

#define FONT_SIZE 126.0
#define ONE_OVER_FONT_SIZE (1.0 / FONT_SIZE)
#define ONE_OVER_BITMAP_SIZE (1.0 / 1024.0)

#include "Data\GlobalData.glsl"
#include "Data\GlyphInstance.glsl"

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_UV;

layout(location = 0) out vec2 v_UV;
layout(location = 1) out vec3 v_Normal;
layout(location = 2) out vec4 v_Color;


void main()
{
    GlyphInstance instance = Instances[gl_InstanceIndex];

    vec4 scaledPosition = vec4(a_Position.xy * instance.Rect.zw, 0.0, 1.0);

    gl_Position = GlobalData.OrthographicViewProjection * instance.Model * scaledPosition;

    // Pass to fragment shader
    v_UV = instance.Rect.xy * ONE_OVER_BITMAP_SIZE + a_UV * instance.Rect.zw * ONE_OVER_BITMAP_SIZE;
    v_Normal = a_Normal;
    v_Color = instance.Color;
}