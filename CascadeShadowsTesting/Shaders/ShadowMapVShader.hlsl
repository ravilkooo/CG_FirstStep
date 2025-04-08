struct VS_IN
{
    float3 pos : POSITION0;
    float4 col : COLOR0;
};

struct PS_IN
{
    float4 pos : SV_POSITION;
    float4 col : COLOR;
};

cbuffer CBuf : register(b0)
{
    row_major float4x4 wMat;
    row_major float4x4 vpMat;
};

PS_IN VSMain(VS_IN input)
{
    PS_IN output = (PS_IN) 0;
	
    output.pos = mul(mul(float4(input.pos, 1.0), wMat), vpMat);
    output.col = input.col;
	
    return output;
}
