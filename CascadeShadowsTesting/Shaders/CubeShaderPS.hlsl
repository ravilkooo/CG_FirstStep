Texture2D ShadowMap : register(t0);
SamplerState Sampler : register(s0)
{
    // Filter = MIN_MAG_MIP_LINEAR;
    // AddressU = WRAP;
    // AddressV = WRAP;
};

SamplerComparisonState samShadow
{
    Filter = COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
    AddressU = BORDER;
    AddressV = BORDER;
    AddressW = BORDER;
    BorderColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

    ComparisonFunc = LESS;
};
 
struct PS_IN
{
    float4 pos : SV_POSITION;
    float4 col : COLOR;
    float3 wPos : POSITION;
};

cbuffer CBuf
{
    row_major float4x4 wMat;
    row_major float4x4 vpMatLightView;
};

float4 PSMain(PS_IN input) : SV_Target
{
    float4 shPos = mul(float4(input.wPos, 1.0), vpMatLightView);
    shPos = shPos / shPos.w;
    
    float4 col = input.col; // * min((1 - input.pos.z)/0.1, 1); //sqrt
    if ((shPos.x > -1) && (shPos.y > -1) && (shPos.z > 0) && (shPos.x < 1) && (shPos.y < 1) && (shPos.z < 1))
    {
        shPos.xy = float2(0.5 * shPos.x + 0.5, -0.5 * shPos.y + 0.5);
        float shZ = ShadowMap.Sample(Sampler, shPos.xy);
        if (shZ < shPos.z)
            // return float4(col.xyz * shZ, col.a);
            // return col;
            return shZ.rrrr;
        else 
            return col;
    }
    else 
        return col;
    
}