layout(std140, set = 0, binding = 0) uniform u_GlobalData
{
    // Screen width, screen height, camera near, camera far
    vec4 ScreenSize;
    mat4 PerspectiveViewProjection;
    mat4 OrthographicViewProjection;
    vec4 SkyColor;
    vec4 LightColor;
    vec3 LightDirection;
    float _padding0;
    vec2 FogParams;
    vec2 _padding1;
    uint RenderMode;
    float Time;
    vec2 _padding2;
} GlobalData;