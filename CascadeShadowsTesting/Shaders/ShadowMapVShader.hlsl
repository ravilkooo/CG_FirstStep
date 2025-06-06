struct VS_IN
{
    float3 pos : POSITION0;
    float4 col : COLOR0;
    float2 texCoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

struct PS_IN
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL0;
};

cbuffer CBuf : register(b0)
{
    row_major float4x4 wMat;
    row_major float4x4 wMatInvTranspose;
    row_major float4x4 vpMat;
};

cbuffer ShadowCBuf : register(b1)
{
    row_major float4x4 lightView;
    row_major float4x4 lightProj;
    row_major float4x4 shadowTransform;
};


PS_IN VSMain(VS_IN input)
{
    PS_IN output = (PS_IN) 0;
	
    output.pos = mul(mul(mul(float4(input.pos, 1.0), wMat), lightView), lightProj);
    output.pos = output.pos / output.pos.w;
    output.normal = normalize(mul(float4(input.normal, 0), wMatInvTranspose));
    return output;
}
