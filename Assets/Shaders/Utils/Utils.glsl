float RemapNormalChannel(float color)
{
    if (sign(color) < 0) color *= 0.1;
    return abs(color);
}

const vec3 TriangleColors[6] = vec3[](
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0),
    vec3(1.0, 1.0, 0.0),
    vec3(1.0, 0.0, 1.0),
    vec3(0.0, 1.0, 1.0)
);

vec4 GetTriangleColor(uint index)
{
    return vec4(TriangleColors[index % 6], 1.0);
}