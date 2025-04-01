Texture2D DiffuseMap : register(t0);
SamplerState Sampler : register(s0)
{
    AddressU = WRAP;
    AddressV = WRAP;
};

struct DirectionalLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float3 Direction;
    float pad;
};

struct Material
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular; // w = SpecPower
    float4 Reflect;
};
cbuffer CollectibleCBuf
{
    float3 cam_pos;
};

struct PS_IN
{
    float4 pos : SV_POSITION;
    float4 col : COLOR;
    float2 texCoord : TEXCOORD;
    float3 normal : NORMAL0;
    float3 wPos : POSITION;
};

float4 PSMain(PS_IN input) : SV_Target
{
    float4 pixelColor = DiffuseMap.Sample(Sampler, input.texCoord);
    
    float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);
    Material mat;
    // mat.Ambient = float4(0.48f, 0.47f, 0.46f, 1.0f);
    mat.Ambient = pixelColor;
    // mat.Diffuse = float4(0.48f, 0.77f, 0.46f, 1.0f);
    mat.Diffuse = pixelColor;
    mat.Specular = float4(0.9f, 0.9f, 0.9f, 16.0f);
    
    DirectionalLight L;
    L.Ambient = float4(0.2f, 0.2f, 0.2f, 0.0f);
    L.Diffuse = float4(0.8f, 0.8f, 0.8f, 0.0f);
    L.Specular = float4(0.9f, 0.9f, 0.9f, 0.0f);
    L.Direction = float4(normalize(float3(-2.0f, -1.0f, -1.0f)), 0.0f);

    float3 normal = normalize(input.normal);
    
    float3 toEye = normalize(cam_pos - input.wPos);
    float3 lightVec = -L.Direction;
    
    ambient = mat.Ambient * L.Ambient;
    float diffuseFactor = dot(lightVec, normal);
    [flatten]
    if (diffuseFactor > 0.0f)
    {
        float3 v = reflect(-lightVec, normal);
        float specFactor = pow(max(dot(v, toEye), 0.0f), mat.Specular.w);
        diffuse = diffuseFactor * mat.Diffuse * L.Diffuse;
        spec = specFactor * mat.Specular * L.Specular;
    }
    
    return float4(ambient + spec + diffuse);
}