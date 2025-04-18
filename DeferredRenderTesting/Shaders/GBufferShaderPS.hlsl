struct PS_IN
{
    float4 pos : SV_POSITION;
    float4 col : COLOR;
    float3 normal : NORMAL0;
    float3 wPos : POSITION;
};

struct PSOutput
{
    float4 Normal : SV_Target0;
    float4 Albedo : SV_Target1;
    float2 Specular : SV_Target2;
};

PSOutput PSMain(PS_IN input)
{
    PSOutput output;
    output.Normal = float4(normalize(input.normal), 1.0);
    output.Albedo = input.col;
    output.Specular = float2(
        saturate(dot(input.col.xyz, float(1).xxx).x * 2),
        10);
    return output;
}