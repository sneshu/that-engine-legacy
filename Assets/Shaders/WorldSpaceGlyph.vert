#version 450 core

#define FONT_SIZE 126.0
#define ONE_OVER_FONT_SIZE (1.0 / FONT_SIZE)
#define ONE_OVER_BITMAP_SIZE (1.0 / 1024.0)

#include "Data\GlobalData.glsl"
#include "Data\GlyphInstance.glsl"
#include "Data\RenderMode.glsl"
#include "Utils\Utils.glsl"

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_UV;

layout(location = 0) out vec2 v_UV;
layout(location = 1) out vec3 v_Normal;
layout(location = 2) out vec4 v_Color;

void main()
{
    GlyphInstance instance = Instances[gl_InstanceIndex];

    vec2 scale = instance.Rect.zw * ONE_OVER_FONT_SIZE;
    vec3 scaledPosition = a_Position * vec3(scale.x, scale.y, 1.0);

    gl_Position = GlobalData.PerspectiveViewProjection * instance.Model * vec4(scaledPosition, 1.0);

    // Pass to fragment shader
    v_UV = instance.Rect.xy * ONE_OVER_BITMAP_SIZE + a_UV * instance.Rect.zw * ONE_OVER_BITMAP_SIZE;
    v_Normal = mat3(instance.Model) * a_Normal;

    // Triangles mode
    if (GlobalData.RenderMode == RENDER_MODE_TRIANGLES)
    {
        v_Color = GetTriangleColor(gl_VertexIndex / 3);
    }

    // Color mode
    else
    {
        v_Color = instance.Color;
    }
}