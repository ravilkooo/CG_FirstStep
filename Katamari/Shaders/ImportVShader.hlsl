float HeightFunc(float3 pos)
{
    return max(0, 2 * sin(pos.x * 0.5) * sin(pos.z * 0.5));
}

struct VS_IN
{
    float3 pos : POSITION0;
    float4 col : COLOR0;
    float2 texCoord : TEXCOORD0;
};

struct PS_IN
{
    float4 pos : SV_POSITION;
    float4 col : COLOR;
    float2 texCoord : TEXCOORD;
};

cbuffer CBuf
{    
    row_major float4x4 wMat;
    row_major float4x4 vpMat;
    float3 center_position;
};

PS_IN VSMain(VS_IN input)
{
    PS_IN output = (PS_IN) 0;
	
    output.pos = mul(float4(input.pos, 1.0), wMat);
    output.pos.y += HeightFunc(center_position);
    output.pos = mul(output.pos, vpMat);
    output.col = input.col;
    output.texCoord = input.texCoord;
	
    return output;
}
