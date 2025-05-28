uint wang_hash(uint seed)
{
    seed = (seed ^ 61) ^ (seed >> 16);
    seed *= 9;
    seed = seed ^ (seed >> 4);
    seed *= 0x27d4eb2d;
    seed = seed ^ (seed >> 15);
    return seed;
}

uint rand_xorshift(uint rng_state)
{
    // Xorshift algorithm from George Marsaglia's paper
    rng_state ^= (rng_state << 13);
    rng_state ^= (rng_state >> 17);
    rng_state ^= (rng_state << 5);
    return rng_state;
}

//random float number between 0-1
float rand_xorshift_normalized(uint rng_state)
{
    return float(rand_xorshift(rng_state)) * (1.0 / 4294967296.0);
}


cbuffer pointLightConstantBuffer : register(b0)
{
    float dt;
    float rngSeed;
    uint pointLightCount;
    
    float padding;
};

struct PointLight
{
    float4 Diffuse;
    float4 Specular;
    float3 Position;
    float Range;
    float3 Att;
    float pad;
};

struct PointLightMovement
{
    float3 Velocity;
    float pad;
    float3 Acceleration;
    float pad2;
};

RWStructuredBuffer<PointLight> pointLightList : register(u0);
RWStructuredBuffer<PointLightMovement> pointLightMovement : register(u1);

//256 particles per thread group
[numthreads(256, 1, 1)]
void main(uint3 id : SV_DispatchThreadID, uint groupId : SV_GroupIndex) //SV_GroupIndex is a flatenned index
{
    uint plIndex = id;
    //first thread to initialise the arguments
    if (plIndex < pointLightCount)
    {
        uint rng_state = wang_hash(id.x + rngSeed);
        uint rng_state_2 = wang_hash(id.x + 7 * rngSeed);
        
        float rotation = rand_xorshift_normalized(rng_state) * 2.0f * 3.14159265f;
        float rotation2 = rand_xorshift_normalized(rng_state_2) * 3.14159265f - 3.14159265f * 0.5f;
        
        PointLight pl = pointLightList[plIndex];
        PointLightMovement pmv = pointLightMovement[plIndex];
        
        float3 da = float3(cos(rotation) * cos(rotation2), sin(rotation2), sin(rotation) * cos(rotation2));

        pmv.Acceleration = normalize(pmv.Acceleration + da * dt);
        pmv.Velocity = normalize(pmv.Velocity + pmv.Acceleration * dt);
        pl.Position += 10 * pmv.Velocity * dt;        
        
        if (pl.Position.x >= 40)
            pl.Position.x -= 80;
        else if (pl.Position.x <= -40)
            pl.Position.x += 80;
        
        if (pl.Position.z >= 40)
            pl.Position.z -= 80;
        else if (pl.Position.z <= -40)
            pl.Position.z += 80;
        
        if (pl.Position.y >= pl.Range + 10)
            pl.Position.y -= 2 * pl.Range + 10;
        else if (pl.Position.y <= -pl.Range)
            pl.Position.y += 2 * pl.Range + 10;
        
        pointLightMovement[plIndex] = pmv;
        pointLightList[plIndex] = pl;

    }

}
