struct GlyphInstance
{
    mat4 Model;
    vec4 Rect;
    vec4 Color;
};

layout(std430, binding = 1) readonly buffer InstanceBuffer
{
    GlyphInstance Instances[];
};