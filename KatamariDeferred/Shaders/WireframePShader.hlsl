struct PS_IN
{
    float4 pos : SV_POSITION;
};

float4 PSMain(PS_IN input) : SV_Target
{
    return float4(1, 1, 0, 1);
}
