#include "FloorHeightFunc.hlsl"

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
    float4 col : COLOR;
    float2 texCoord : TEXCOORD;
    float3 normal : NORMAL0;
    float3 wPos : POSITION;
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
	
    output.pos = mul(float4(input.pos.x, FloorHeightFunc(input.pos), input.pos.z, 1.0), wMat);
    output.wPos = output.pos.xyz / output.pos.w;
    
    output.pos = mul(output.pos, vpMat);
    output.col = input.col;
    output.texCoord = input.texCoord;
    output.normal = mul(float4(input.normal, 0), wMat);
    
    output.normal = -normalize(float3(
    (FloorHeightFunc(input.pos + float3(0.01, 0, 0)) - FloorHeightFunc(input.pos)) * 100,
    -1,
    (FloorHeightFunc(input.pos + float3(0, 0, 0.01)) - FloorHeightFunc(input.pos)) * 100
    ));
	
    return output;
}
