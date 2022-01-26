#version 450

layout(location = 0) in float near; //0.01
layout(location = 1) in float far; //100
layout(location = 2) in vec3 nearPoint;
layout(location = 3) in vec3 farPoint;
layout(location = 4) in mat4 fragView;
layout(location = 8) in mat4 fragProj;

layout(location = 0) out vec4 outColor;

vec4 grid(vec3 fragPos3D, float scale, float opacityMod, bool drawAxis) 
{
    vec2 coord = fragPos3D.xz * scale;
    vec2 derivative = fwidth(coord);
    vec2 grid = abs(fract(coord - 0.5) - 0.5) / derivative;
    float line = min(grid.x, grid.y);
    float minimumz = min(derivative.y, 1);
    float minimumx = min(derivative.x, 1);
    vec4 color = vec4(0.2, 0.2, 0.2, 1.0 - min(line, 1.0));

    // z axis
    if(fragPos3D.x > -1.0 * minimumx && fragPos3D.x < minimumx)
    {
        color.b = 1.0;
        color.g = 0.01;
        color.r = 0.01;
        color.a = 1.0;
    }
    // x axis
    if(fragPos3D.z > -1.0 * minimumz && fragPos3D.z < minimumz)
    {
        color.r = 1.0;
        color.g = 0.01;
        color.b = 0.01;
        color.a = 1.0;
    }
    return color;
}

float computeDepth(vec3 pos) 
{
    vec4 clip_space_pos = fragProj * fragView * vec4(pos.xyz, 1.0);
    return (clip_space_pos.z / clip_space_pos.w);
}

float computeLinearDepth(vec3 pos) 
{
    vec4 clip_space_pos = fragProj * fragView * vec4(pos.xyz, 1.0);
    float clip_space_depth = (clip_space_pos.z / clip_space_pos.w) * 2.0 - 1.0; // put back between -1 and 1
    float linearDepth = (2.0 * near * far) / (far + near - clip_space_depth * (far - near)); // get linear value between 0.01 and 100
    return linearDepth / far; // normalize
}

void main() 
{
    float t = -nearPoint.y / (farPoint.y - nearPoint.y);
    vec3 fragPos3D = nearPoint + t * (farPoint - nearPoint);

    gl_FragDepth = computeDepth(fragPos3D);

    float linearDepth = computeLinearDepth(fragPos3D);
    float fading = max(0, (0.25 - linearDepth));

    vec4 largeGridCol = grid(fragPos3D, 1,1, true);

    outColor = (largeGridCol)* float(t > 0);
    outColor.a *= fading;
}