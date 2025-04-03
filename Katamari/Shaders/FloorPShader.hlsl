#include "FloorHeightFunc.hlsl"

Texture2D DiffuseMap : register(t0);
SamplerState Sampler : register(s0)
{
    AddressU = BORDER;
    AddressV = BORDER;
};

struct PS_IN
{
    float4 pos : SV_POSITION;
    float4 col : COLOR;
    float2 texCoord : TEXCOORD;
    float3 normal : NORMAL0;
    float3 wPos : POSITION;
};

struct Material
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float4 Reflect;
};

struct DirectionalLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    
    float3 Direction;
    float pad;
};

struct PointLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    
    float3 Position;
    float Range;
    
    float3 Att;
    float pad;
};

struct SpotLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float3 Position;
    float Range;
    
    float3 Direction;
    float Spot;
    
    float3 Att;
    float pad;
};

cbuffer LightBuffer : register(b0) // per frame
{
    DirectionalLight dLight;
    PointLight pointLights[14];
    SpotLight sLight;
};

cbuffer FloorCBuf : register(b1) // per object
{
    float3 cam_pos;
};


float4 calcDirectLight(float3 wPos, float3 normal, float3 toEye, Material mat, DirectionalLight dirLight)
{
    float4 dl_ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 dl_diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 dl_spec = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    dl_ambient = mat.Ambient * dirLight.Ambient;
    
    {
        float3 lightVec = -dirLight.Direction;
        float diffuseFactor = dot(lightVec, normal);
        [flatten]
        if (diffuseFactor > 0.0f)
        {
            float3 v = reflect(-lightVec, normal);
            float specFactor = pow(max(dot(v, toEye), 0.0f), mat.Specular.w);
            dl_diffuse = diffuseFactor * mat.Diffuse * dirLight.Diffuse;
            dl_spec = specFactor * mat.Specular * dirLight.Specular;
        }
    }
    
    return float4(saturate(dl_ambient + dl_spec + dl_diffuse).rgb, 1.0f);
}

float4 calcPointLight(float3 wPos, float3 normal, float3 toEye, Material mat, PointLight pointLight)
{
    float4 pl_ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 pl_diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 pl_spec = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    float3 lightVec = pointLight.Position - wPos;
    float d = length(lightVec);
       
    if (d > pointLight.Range)
        return float4(0, 0, 0, 0);
    lightVec /= d;
    pl_ambient = mat.Ambient * pointLight.Ambient;
    float diffuseFactor = dot(lightVec, normal);
        
    [flatten]
    if (diffuseFactor > 0.0f)
    {
        float3 v = reflect(-lightVec, normal);
        float specFactor = pow(max(dot(v, toEye), 0.0f), mat.Specular.w);
        pl_diffuse = diffuseFactor * mat.Diffuse * pointLight.Diffuse;
        pl_spec = specFactor * mat.Specular * pointLight.Specular;
    }

    float att = 1.0f / dot(pointLight.Att, float3(1.0f, d, d * d));
    pl_diffuse *= att;
    pl_spec *= att;
        
    return saturate(pl_ambient + pl_diffuse + pl_spec);
}

float4 calcSpotLight(float3 wPos, float3 normal, float3 toEye, Material mat, SpotLight spotLight)
{
    float4 sl_ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 sl_diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 sl_spec = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    float3 lightVec = spotLight.Position - wPos;
    float d = length(lightVec);
       
    if (d > spotLight.Range)
        return float4(0, 0, 0, 0);
    lightVec /= d;
    sl_ambient = mat.Ambient * spotLight.Ambient;
    float diffuseFactor = dot(lightVec, normal);
        
    [flatten]
    if (diffuseFactor > 0.0f)
    {
        float3 v = reflect(-lightVec, normal);
        float specFactor = pow(max(dot(v, toEye), 0.0f), mat.Specular.w);
        sl_diffuse = diffuseFactor * mat.Diffuse * spotLight.Diffuse;
        sl_spec = specFactor * mat.Specular * spotLight.Specular;
    }
    
    float spot = pow(max(dot(-lightVec, spotLight.Direction), 0.0f), 160);
    

    float att = 1.0f / dot(spotLight.Att, float3(1.0f, d, d * d));
    sl_ambient *= spot;
    sl_diffuse *= att;
    sl_spec *= att;
        
    return saturate(sl_ambient + sl_diffuse + sl_spec);
}


float4 PSMain(PS_IN input) : SV_Target
{
    float4 pixelColor = DiffuseMap.Sample(Sampler, input.texCoord);
    Material mat = {
        pixelColor,
        pixelColor,
        pixelColor * 0.2f,
        { 0.0f, 0.0f, 0.0f, 0.0f }
    };
    
    float3 normal = normalize(input.normal);
    float3 toEye = normalize(cam_pos - input.wPos);
 
    float4 dirLightCol = calcDirectLight(input.wPos, normal, toEye, mat, dLight);

    float4 pointLightSum = { 0, 0, 0, 0 };

    for (int i = 0; i < 14; i++)
    {
        pointLightSum += calcPointLight(input.wPos, normal, toEye, mat, pointLights[i]);
    }
    
    //float4 spotLightCol = calcSpotLight(input.wPos, normal, toEye, mat , sLight);
    
    return saturate(dirLightCol + pointLightSum);
    
}