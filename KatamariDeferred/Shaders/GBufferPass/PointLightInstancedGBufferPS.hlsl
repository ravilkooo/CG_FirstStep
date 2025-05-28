struct PointLight
{
    float4 Diffuse : COLOR1;
    float4 Specular : COLOR2;
    float3 Position : POSITION1;
    float Range : TEXCOORD1;

    float3 Att : POSITION2;
    float pad : TEXCOORD2;
};

struct PS_IN
{
    float4 pos : SV_POSITION;
    float4 col : COLOR0;
    float2 texCoord : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 wPos : POSITION0;
    
    PointLight pointLightData;
};

struct PSOutput
{
    float4 Normal : SV_Target0;
    float4 Albedo : SV_Target1;
    float2 Specular : SV_Target2;
    float4 WorldPos : SV_Target3;
};

PSOutput PSMain(PS_IN input) : SV_Target
{
    PSOutput output;
    output.Normal = float4(normalize(input.normal), 1.0);
    output.Albedo = input.pointLightData.Diffuse;
    output.Specular = float2(0, 1);
    output.WorldPos = float4(input.wPos, 1.0f);
    return output;
}