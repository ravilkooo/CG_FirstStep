Texture2D shadowMap : register(t0);
SamplerState shadowSampler : register(s0);

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
        // [flatten]
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
        
    // [flatten]
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
    DirectionalLight directionalLight;
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

static const float SMAP_SIZE = 512.0f;
static const float SMAP_DX = 1.0f / SMAP_SIZE;

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
    
    float4 dl_ambient;
    float4 dl_diffuse;
    float4 dl_spec;
    
    calcDirectionalLight(input.wPos, normal, toEye, mat, directionalLight, dl_ambient, dl_diffuse, dl_spec);
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
        
        /*
        float depth = shPos.z;
        
        float s0 = shadowMap.Sample(shadowSampler, shPos.xy).r;
        float s1 = shadowMap.Sample(shadowSampler, shPos.xy + float2(SMAP_DX, 0)).r;
        float s2 = shadowMap.Sample(shadowSampler, shPos.xy + float2(0, SMAP_DX)).r;
        float s3 = shadowMap.Sample(shadowSampler, shPos.xy + float2(SMAP_DX, SMAP_DX)).r;
        // Is the pixel depth <= shadow map value?
        float result0 = depth <= s0;
        float result1 = depth <= s1;
        float result2 = depth <= s2;
        float result3 = depth <= s3;
        // Transform to texel space.
        float2 texelPos = SMAP_SIZE * shPos.xy; // Determine the interpolation amounts.
        float2 t = frac(texelPos + 0.5);
        // Interpolate results.
        float percentLit = lerp(lerp(result0, result1, t.x), lerp(result2, result3, t.x), t.y);
        return saturate(dl_ambient + dl_diffuse * percentLit + dl_spec * percentLit);
        // return float4(((floor(shPos.x * SMAP_SIZE) + floor(shPos.y * SMAP_SIZE)) % 2).xxx * percentLit, 1.0f); // DEBUG_MODE
        */
        // shPos.xy = float2(0.5 * shPos.x + 0.5, -0.5 * shPos.y + 0.5);
        float shZ = shadowMap.Sample(shadowSampler, shPos.xy).r;
        if (shZ < shPos.z)
        {
            // float percentLit = 1.0f;
            // percentLit = shadowMap.SampleCmpLevelZero(shadowSampler, shPos.xy, shPos.z).r;
            return float4(((floor(shPos.x * SMAP_SIZE) + floor(shPos.y * SMAP_SIZE)) % 2).xxx, 1.0f); // DEBUG_MODE
            // return saturate(dl_ambient);
        }
        else 
            return saturate(dl_ambient + dl_diffuse + dl_spec);
    }
    else 
        return saturate(dl_ambient + dl_diffuse + dl_spec);
    
}