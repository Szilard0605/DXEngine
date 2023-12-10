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
    
    output.Position = mul(ViewProjectionMatrix, float4(input.Position, 1.0));
  
    output.Color = input.Color;
    return output;
}

float4 PS_Main(VS_OUTPUT input) : SV_Target
{
    return input.Color;
}