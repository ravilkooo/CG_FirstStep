Texture2D DiffuseMap : register(t0);
SamplerState Sampler : register(s0)
{
    AddressU = WRAP;
    AddressV = WRAP;
};

struct PS_IN
{
    float4 pos : SV_POSITION;
    float4 col : COLOR;
    float2 texCoord : TEXCOORD;
};

float4 PSMain(PS_IN input) : SV_Target
{
    // float4 col = input.col;
    // float4 col = DiffuseMap.Sample(Sampler, input.tex.xy);
    //if (input.pos.x > 400)
    //    col = float4(0.0f, 1.0f, 0.0f, 1.0f);

    float3 pixelColor = DiffuseMap.Sample(Sampler, (input.texCoord.xy));
    return float4(pixelColor, 1);
}