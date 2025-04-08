Texture2D ShadowMap : register(t0);
SamplerState Sampler : register(s0)
{
};

/*SamplerComparisonState samShadow
{
    Filter = COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
    // Return 0 for points outside the light frustum
    // to put them in shadow.
    AddressU = BORDER;
    AddressV = BORDER;
    AddressW = BORDER;
    BorderColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
    ComparisonFunc = LESS;
};*/


struct Material
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular; // w = SpecPower
    float4 Reflect;
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

struct DirectionalLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    
    float3 Direction;
    float pad;
};


void calcDirectLight(float3 wPos, float3 normal, float3 toEye, Material mat, DirectionalLight dirLight,
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
    
    return;
}


void calcPointLight(float3 wPos, float3 normal, float3 toEye, Material mat, PointLight pointLight,
    out float4 pl_ambient,
    out float4 pl_diffuse,
    out float4 pl_spec)
{
    pl_ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    pl_diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    pl_spec = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    float3 lightVec = pointLight.Position - wPos;
    float d = length(lightVec);
       
    if (d > pointLight.Range)
        return;
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
        
    return;
}

 
struct PS_IN
{
    float4 pos : SV_POSITION;
    float4 col : COLOR;
    float3 normal : NORMAL0;
    float3 wPos : POSITION;
};

cbuffer LightCBuf : register(b0) // per frame
{
    PointLight pointLight;
};

cbuffer CamCBuf : register(b1)
{
    float3 cam_pos;
};

cbuffer ShadowCBuf : register(b2)
{
    row_major float4x4 lightView;
    row_major float4x4 lightProj;
    row_major float4x4 shadowTransform;
};


float4 PSMain(PS_IN input) : SV_Target
{
    
    float4 pixelColor = input.col;
    Material mat =
    {
        pixelColor,
        pixelColor,
        float4(0.9, 0.9, 0.9, 16.0f),
        { 0.0f, 0.0f, 0.0f, 0.0f }
    };
    
    float3 normal = normalize(input.normal);
    float3 toEye = normalize(cam_pos - input.wPos);
    
    float4 pl_ambient;
    float4 pl_diffuse;
    float4 pl_spec;
    
    calcPointLight(input.wPos, normal, toEye, mat, pointLight, pl_ambient, pl_diffuse, pl_spec);
    /*
    calcDirectLight(input.wPos, normal, toEye, mat, dLight, ...);

    float4 pointLightSum = { 0, 0, 0, 0 };
    
    for (int i = 0; i < 14; i++)
    {
        calcPointLight(input.wPos, normal, toEye, mat, pointLights[i], ...);
    }
    
    return saturate(dirLightCol + pointLightSum);
    
    */
    
    float4 shPos = mul(float4(input.wPos, 1.0), shadowTransform);
    shPos = shPos / shPos.w;
    
    if ((shPos.x >= 0) && (shPos.y >= 0) && (shPos.z >= 0) && (shPos.x <= 1) && (shPos.y <= 1) && (shPos.z <= 1))
    {
        // shPos.xy = float2(0.5 * shPos.x + 0.5, -0.5 * shPos.y + 0.5);
        float shZ = ShadowMap.Sample(Sampler, shPos.xy);
        if (shZ < shPos.z)
        {
            // float percentLit = 1.0f;
            // percentLit = ShadowMap.SampleCmpLevelZero(samShadow, shPos.xy, shPos.z).r;
            return float4(((floor(shPos.x * 100) + floor(shPos.y * 100)) % 2).xxx, 1.0f); // DEBUG_MODE
            // return saturate(pl_ambient);
        }
        else 
            return saturate(pl_ambient + pl_diffuse + pl_spec);
    }
    else 
        return saturate(pl_ambient + pl_diffuse + pl_spec);
    
}