#version 450 core

#include "Data\GlobalData.glsl"
#include "Data\MeshInstance.glsl" 
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
    MeshInstance instance = Instances[gl_InstanceIndex];
    gl_Position = GlobalData.PerspectiveViewProjection * instance.Model * vec4(a_Position, 1.0);
    v_UV = a_UV;
    v_Normal = mat3(instance.Model) * a_Normal;

    // Triangles mode
    if (GlobalData.RenderMode == RENDER_MODE_TRIANGLES)
    {
        v_Color = GetTriangleColor(gl_VertexIndex / 3);
    }
}