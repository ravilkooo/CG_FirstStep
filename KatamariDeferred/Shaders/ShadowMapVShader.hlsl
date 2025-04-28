struct VS_IN
{
    float3 pos : POSITION0;
};

struct PS_IN
{
    float4 pos : SV_POSITION;
};

cbuffer ShadowCBuf : register(b0) // per frame
{
    row_major float4x4 lightView;
    row_major float4x4 lightProj;
    row_major float4x4 shadowTransform;
};

cbuffer ConstantBuffer : register(b1) // per object
{
    row_major float4x4 wMat;
    row_major float4x4 wMatInvTranspose;
    row_major float4x4 vpMat;
};


PS_IN VSMain(VS_IN input)
{
    PS_IN output = (PS_IN) 0;
	
    output.pos = mul(mul(mul(float4(input.pos, 1), wMat), lightView), lightProj);
    output.pos = output.pos / output.pos.w;
    return output;
}
