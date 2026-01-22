// VERTEX SHADER

struct VS_INPUT
{
    float3 Position : a_position;
    float3 Normal : a_normal;
    float3 Tangent : a_tangent;
    float3 Bitangent : a_bitangent;
    float2 TexCoords : a_texcoords;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float3 WorldPos : TEXCOORD0;
    float3 Normal : TEXCOORD1;
    float2 TexCoords : TEXCOORD2;
    float3 Binormal : BINORMAL;
    float3 Tangent : TANGENT;
};

cbuffer RenderData : register(b0)
{
    float4x4 ViewProjectionMatrix;
    float4x4 Transform;
};


VS_OUTPUT VS_Main(VS_INPUT input)
{
    VS_OUTPUT output;
    
    float4 worldPos = mul(Transform, float4(input.Position, 1.0));
    output.Position = mul(ViewProjectionMatrix, worldPos);
    output.WorldPos = worldPos.xyz;
    
    float3x3 wsTransform = (float3x3) Transform;
    output.Normal   = normalize(mul(input.Normal, wsTransform));
    output.Tangent  = normalize(mul(input.Tangent, wsTransform));
    output.Binormal = normalize(mul(input.Bitangent, wsTransform));
    
    
    output.TexCoords = input.TexCoords;
    return output;
}

// FRAGMENT SHADER

Texture2D baseColorTexture : register(t0); 
SamplerState baseColorSampler : register(s0); 

Texture2D normalTexture : register(t1);
SamplerState normalSampler : register(s1);

Texture2D metallicTexture : register(t2);
SamplerState metallicSampler : register(s2);

Texture2D roughnessTexture : register(t3);
SamplerState roughnessSampler : register(s3);

TextureCube environmentMap : register(t4);
SamplerState environmentSampler : register(s4);

cbuffer LightData : register(b1)
{
    float AmbientLightIntensity;
    float3 DirectionalLightDirection; // normalized, points *toward* surface

    float3 DirectionalLightColor;
    float _pad1;

    float3 CameraPosition;
    float _pad2;
};

cbuffer MaterialData : register(b2)
{
    float3 BaseColor;
    float Metallic;
    float Roughness;
    float Specular;
    float Padding;

    uint hasAlbedoMap = 0;
    uint hasNormalMap = 0;
    uint hasMetallicRougnessTexture = 0;
    
    float padding1;
    float padding2;
};

static const float PI = 3.14159265359;

float DistributionGGX(float3 N, float3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    return a2 / (PI * denom * denom);
}


float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;
    return NdotV / (NdotV * (1.0 - k) + k);
}

float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    return GeometrySchlickGGX(NdotV, roughness) *
           GeometrySchlickGGX(NdotL, roughness);
}

float3 FresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float3 NormalMap(VS_OUTPUT input)
{
    float3 T = normalize(input.Tangent);
    float3 N = normalize(input.Normal);
    float3 B = normalize(input.Binormal); 
    
    float3x3 TBN = float3x3(T, B, N);
    float3 normalMap = normalTexture.Sample(normalSampler, input.TexCoords).rgb * 2.0 - 1.0;
    normalMap = normalize(mul(normalMap.rgb, TBN));
    return normalMap;
}


float4 PS_Main(VS_OUTPUT input) : SV_Target
{
    // --- Normal ---
    float3 N = normalize(input.Normal);
    if (hasNormalMap == 1)
        N = normalize(NormalMap(input));
    
    // --- Textures / constants ---
    float3 albedo = hasAlbedoMap ? baseColorTexture.Sample(baseColorSampler, input.TexCoords).rgb : BaseColor;

    float metallic = Metallic;
    float roughness = Roughness;

    if (hasMetallicRougnessTexture)
    {
        float4 mrSample = metallicTexture.Sample(metallicSampler, input.TexCoords);
        metallic = mrSample.b;
        roughness = mrSample.g;
    }

    roughness = max(roughness, 0.1);
    float3 V = normalize(CameraPosition - input.WorldPos);
    float3 L = normalize(-DirectionalLightDirection);

    float NdotV = max(dot(N, V), 0.0001); // Prevent division by zero
    float NdotL = max(dot(N, L), 0.0);
    
    float3 H = normalize(V + L);
    
    float3 F0 = lerp(float3(0.04, 0.04, 0.04), albedo, metallic);
    float D = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    float3 F = FresnelSchlick(saturate(dot(H, V)), F0);

    float clampNdotV = max(dot(N, V), 0.001);
    float clampNdotL = max(dot(N, L), 0.001);

    float3 numerator = D * G * F;
    float denominator = 4.0 * clampNdotV * clampNdotL;
    float3 specular = numerator / max(denominator, 0.001);
    
    float3 kS = F;
    float3 kD = (1.0 - kS) * (1.0 - metallic);
    
    float3 radiance = DirectionalLightColor;
    float3 diffuse = kD * albedo / PI;

    float3 ambient = AmbientLightIntensity * albedo; 
    float3 color = ambient + (diffuse + specular) * radiance * NdotL;
    
  
    // --- Tonemap + gamma ---
    color = color / (color + 1.0);
    color = pow(color, 1.0 / 2.2);
    return float4(color, 1.0);
}

