struct VS_INPUT
{
    float3 Position : a_position;
    float4 Color : a_color;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float4 Color : a_color;
};

cbuffer CamData : register(b0)
{
    float4x4 ViewProjectionMatrix;
};

VS_OUTPUT VS_Main(VS_INPUT input)
{
    VS_OUTPUT output;
    float4 pos = mul(input.Position, ViewProjectionMatrix);
    output.Position = float4(pos.x, pos.y, pos.z, 1.0);
    //output.Position = float4(input.Position.x, input.Position.y, 1.0, 1.0);
    output.Color = input.Color;
    return output;
}

float4 PS_Main(VS_OUTPUT input) : SV_Target
{
    return input.Color;
}