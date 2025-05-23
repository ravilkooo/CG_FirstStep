#include "FloorHeightFunc.hlsl"

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

struct ShadowTransforms
{
    row_major float4x4 lightView;
    row_major float4x4 lightProj;
    row_major float4x4 shadowTransform;
};


cbuffer LightBuffer : register(b0) // per frame
{
    DirectionalLight dLight;
    PointLight pointLights[14];
    SpotLight sLight;
};

cbuffer CascadeCBuf : register(b1) // per frame
{
    ShadowTransforms shTransforms[4];
    float4 distances;
};

cbuffer FloorCBuf : register(b2) // per object
{
    row_major float4x4 viewMat;
    float3 cam_pos;
};

Texture2DArray shadowMap : register(t0);
SamplerState shadowSampler : register(s0);
SamplerComparisonState samShadow : register(s1)
{
    Filter = COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
    AddressU = BORDER;
    AddressV = BORDER;
    AddressW = BORDER;
    BorderColor = float4(1.0f, 1.0f, 1.0f, 0.0f);

    ComparisonFunc = LESS;
};


Texture2D DiffuseMap : register(t1);
SamplerState Sampler : register(s2);

void calcDirectionalLight(float3 wPos, float3 normal, float3 toEye, Material mat, DirectionalLight dirLight,
    out float4 dl_ambient,
    out float4 dl_diffuse,
    out float4 dl_spec)
{
    dl_ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    dl_diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    dl_spec = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
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

static const float SMAP_SIZE = 1024.0f;
static const float SMAP_DX = 1.0f / SMAP_SIZE;

float CalcShadowFactor(SamplerComparisonState samShadow,
                       Texture2DArray shadowMap,
                       float4 shadowPosH,
                       int layer)
{
  // Complete projection by doing division by w.
    shadowPosH.xyz /= shadowPosH.w;
  
  // Depth in NDC space.
    float depth = shadowPosH.z;

    // if (samShadow.Sample())
  
    //return shadowMap.SampleCmpLevelZero(samShadow, shadowPosH.xy, depth).r;
  
    // Texel size.
    const float dx = SMAP_DX;

    float percentLit = 0.0f;
    const float2 offsets[9] =
    {
        float2(-dx, -dx), float2(0.0f, -dx), float2(dx, -dx),
        float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
        float2(-dx, +dx), float2(0.0f, +dx), float2(dx, +dx)
    };

    [unroll]
    for (int i = 0; i < 9; ++i)
    {
        percentLit += shadowMap.SampleCmpLevelZero(samShadow,
        float3(shadowPosH.xy + offsets[i], layer), depth).r;
    }

    return percentLit /= 9.0f;
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
 
    float4 dl_ambient;
    float4 dl_diffuse;
    float4 dl_spec;
    
    float4 dirLightCol;
    
    calcDirectionalLight(input.wPos, normal, toEye, mat, dLight, dl_ambient, dl_diffuse, dl_spec);
    
    /*
    float4 lightViewPos = mul(float4(input.wPos, 1.0), shTransforms[0].lightView);
    lightViewPos = lightViewPos / lightViewPos.w;
    */
    
    int layer = 1;
    
    static float cascadeDistances[4] = (float[4]) distances;
    
    float4 viewPos = mul(float4(input.wPos, 1), viewMat);
    viewPos /= viewPos.w;
    
    for (int i = 0; i < 4; ++i)
    {
        //if (lightViewPos.z < cascadeDistances[i])
        if (viewPos.z < cascadeDistances[i])
        {
            layer = i;
            break;
        }
    }
    
    float4 shPos = mul(float4(input.wPos, 1.0), shTransforms[layer].shadowTransform);
    shPos = shPos / shPos.w;
    
    if ((shPos.x >= 0) && (shPos.y >= 0) && (shPos.z >= 0) && (shPos.x <= 1) && (shPos.y <= 1) && (shPos.z <= 1))
    {
        float shadowFactor = CalcShadowFactor(samShadow, shadowMap, shPos, layer);
        dirLightCol = saturate(dl_ambient + shadowFactor * (dl_diffuse + dl_spec));
    }
    else
        dirLightCol = saturate(dl_ambient + dl_spec + dl_diffuse);

    float4 pointLightSum = { 0, 0, 0, 0 };

    for (int i = 0; i < 14; i++)
    {
        pointLightSum += calcPointLight(input.wPos, normal, toEye, mat, pointLights[i]);
    }
    
    //float4 spotLightCol = calcSpotLight(input.wPos, normal, toEye, mat , sLight);
    
    float4 fColor;
    if (layer == 0)
        fColor = float4(1, 0, 0, 1);
    else if (layer == 1)
        fColor = float4(0, 1, 0, 1);
    else if (layer == 2)
        fColor = float4(0, 0, 1, 1);
    else
        fColor = float4(1, 1, 0, 1);
    
    return saturate(dirLightCol + pointLightSum);
    //return saturate(dirLightCol + pointLightSum) * fColor;
    
}