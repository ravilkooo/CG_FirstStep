struct PointLight
{
    float4 Diffuse : COLOR0;
    float4 Specular : COLOR1;
    float3 Position : POSITION1;
    float Range : TEXCOORD0;

    float3 Att : POSITION2;
    float pad : TEXCOORD1;
};


struct VS_IN
{
    float3 pos : POSITION0;
    
    PointLight pointLightData;
    
    uint InstanceId : SV_InstanceID;
};

struct PS_IN
{
    float4 pos : SV_POSITION;
    float3 wPos : POSITION0;

    PointLight pointLightData;
};

cbuffer CBuf
{
    row_major float4x4 wMat;
    row_major float4x4 wMatInvTranspose;
    row_major float4x4 vpMat;
};

PS_IN VSMain(VS_IN input)
{
    PS_IN output = (PS_IN) 0;
    
    output.pos = float4(input.pos + input.pointLightData.Position, 1.0);
    output.wPos = output.pos.xyz;
    output.pos = mul(output.pos, vpMat);
    
    output.pointLightData = input.pointLightData;
	
    return output;
}
