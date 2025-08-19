struct VS_INPUT
{
    float3 Position : a_position;
    //float4 Color : a_color;
    float2 TexCoords : a_texcoords;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    //float4 Color : a_color;
    float2 TexCoords : a_texcoords;
};

cbuffer RenderData : register(b0)
{
    float4x4 ViewProjectionMatrix;
    float4x4 Transform;
};

VS_OUTPUT VS_Main(VS_INPUT input)
{
    VS_OUTPUT output;
    
    const float4 ViewPos = mul(Transform, float4(input.Position, 1.0));
    output.Position = mul(ViewProjectionMatrix, ViewPos);
    //output.Color = input.Color;
    output.TexCoords = input.TexCoords;
    return output;
}

Texture2D myTexture : register(t0); // Declare the texture variable (register(t0) binds it to the first texture slot)
SamplerState mySampler : register(s0); // Declare the sampler variable (register(s0) binds it to the first sampler slot)


float4 PS_Main(VS_OUTPUT input) : SV_Target
{
    float4 texColor = myTexture.Sample(mySampler, input.TexCoords);
    return texColor;
}