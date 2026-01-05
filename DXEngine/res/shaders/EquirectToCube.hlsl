struct VS_INPUT
{
    float3 Position : a_position;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float3 LocalPosition : TEXCOORD0;
};

VS_OUTPUT VS_Main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.LocalPosition = input.Position;
    output.Position = float4(input.Position, 1.0f);
    return output;
}

// FRAGMENT SHADER

Texture2D EquirectMap : register(t0);
SamplerState sampler0 : register(s0);

cbuffer EquirectData : register(b0)
{
    uint FaceIndex;
}

float3 CubemapDirection(uint face, float3 localPos)
{
    float3 dir = normalize(localPos);

    if (face == 0)
        dir = float3(dir.z, dir.y, -dir.x);
    else if (face == 1)
        dir = float3(-dir.z, dir.y, dir.x);
    else if (face == 2)
        dir = float3(dir.x, dir.z, -dir.y);
    else if (face == 3)
        dir = float3(dir.x, -dir.z, dir.y);
    else if (face == 4)
        dir = float3(dir.x, dir.y, dir.z);
    else if (face == 5)
        dir = float3(-dir.x, dir.y, -dir.z);

    return normalize(dir);
}

float2 SampleSphericalMap(float3 dir)
{
    const float PI = 3.14159265359f;

    float theta = atan2(dir.z, dir.x); 
    float phi = asin(dir.y); 

    float u = (theta + PI) / (2.0f * PI); 
    float v = (phi + PI / 2.0f) / PI; 

    return float2(u,v);
}

float4 PS_Main(VS_OUTPUT input) : SV_Target
{
    float3 dir = CubemapDirection(FaceIndex, input.LocalPosition);
    float2 uv = SampleSphericalMap(dir);
    float3 color = EquirectMap.Sample(sampler0, uv).rgb;
    return float4(color, 1.0f);
}