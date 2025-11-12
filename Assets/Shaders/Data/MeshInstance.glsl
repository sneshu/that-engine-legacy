struct MeshInstance
{
    mat4 Model;
};

layout(std430, binding = 1) readonly buffer InstanceBuffer
{
    MeshInstance Instances[];
};

