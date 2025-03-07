struct VS_IN
{
    float4 pos : POSITION0;
    float4 col : COLOR0;
};

struct PS_IN
{
    float4 pos : SV_POSITION;
    float4 col : COLOR;
};

cbuffer CBuf
{
    row_major float4x4 wvpMat;
};

PS_IN VSMain(VS_IN input)
{
    PS_IN output = (PS_IN) 0;
	
    output.pos = mul(input.pos, wvpMat);
    output.col = input.col;
	
    return output;
}

float4 PSMain(PS_IN input) : SV_Target
{
    float4 col = input.col;
    //if (input.pos.x > 400)
    //    col = float4(0.0f, 1.0f, 0.0f, 1.0f);
    return col;
}