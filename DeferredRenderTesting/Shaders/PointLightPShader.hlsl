struct PS_IN
{
    float4 pos : SV_POSITION;
    float3 wPos : POSITION;
};

float4 PSMain(PS_IN input) : SV_Target
{
    return float4(1.0f, 1.0f, 0.0f, 0.5f);
}