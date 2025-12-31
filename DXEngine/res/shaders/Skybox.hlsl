struct VS_INPUT
{
    float3 Position : a_position;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float3 TexCoords : TEXCOORD;
};

cbuffer RenderData : register(b0)
{
    float4x4 ProjectionMatrix;
    float4x4 ViewMatrix;
};


VS_OUTPUT VS_Main(VS_INPUT input)
{
	VS_OUTPUT output;
	output.TexCoords = float3(input.Position.x, input.Position.y, -input.Position.z);
    float4 pos = mul(mul(ProjectionMatrix, ViewMatrix), float4(input.Position, 1.0));
    output.Position = pos.xyww;
    return output;
}

// FRAGMENT SHADER

TextureCube CubeMap : register(t0); // Declare the texture variable (register(t0) binds it to the first texture slot)
SamplerState sampler0 : register(s0); // Declare the sampler variable (register(s0) binds it to the first sampler slot)

float4 PS_Main(VS_OUTPUT input) : SV_Target
{
    float3 dir = normalize(input.TexCoords);
    return CubeMap.Sample(sampler0, input.TexCoords);
}