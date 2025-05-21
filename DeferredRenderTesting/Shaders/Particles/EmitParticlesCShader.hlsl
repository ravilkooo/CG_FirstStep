#include "ParticlesGlobals.h"

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

cbuffer sceneConstantBuffer : register(b0)
{
    float4 camPosition;
    float dt;
    float rngSeed;
    
    float2 padding;
};

cbuffer deadListCountConstantBuffer : register(b1)
{
    uint nbDeadParticles;

    uint3 deadListPadding;
};

cbuffer emitterPointConstantBuffer : register(b2)
{
    float4 emitterPosition;
    uint emitterMaxSpawn;
    float particlesLifeSpan;
    float2 emitterPadding;
};

ConsumeStructuredBuffer<uint> deadListBuffer : register(u0);
RWStructuredBuffer<Particle> particleList : register(u1);
AppendStructuredBuffer<ParticleIndexElement> aliveParticleIndex : register(u2);
RWBuffer<uint> indirectDispatchArgs : register(u3);

//spawn per batch of 256
[numthreads(256, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    if (id.x < nbDeadParticles && id.x < emitterMaxSpawn)
    {
        uint rng_state = wang_hash(id.x + rngSeed);
        
        Particle p = (Particle) 0;
        p.color = float4(1, 1, 0, 1);
        p.size = 0.2;
        
        p.position = emitterPosition;
        p.position.w = 1.0;

        float colatitude = 2 * 3.1415 * (0.5 - rand_xorshift_normalized(rng_state));
        float longitude = 2 * 3.1415 * (0.5 - rand_xorshift_normalized(rng_state));
        
        float radius = 10.0f;
        p.velocity.x = radius * sin(colatitude) * cos(longitude);
        p.velocity.z = radius * sin(colatitude) * sin(longitude);
        p.velocity.y = radius * cos(colatitude);
        p.velocity.w = 1.0f;
        
        //p.velocity = mul(p.velocity, emitterRotation);
        
        //p.velocity = particlesBaseSpeed * normalize(p.velocity);
        
        p.lifeSpan = particlesLifeSpan;
        p.age = abs(p.lifeSpan); //abs() so if lifetime is infinite ( < 0.0) it's still has a life

        uint index = deadListBuffer.Consume();
        particleList[index] = p;

        ParticleIndexElement pe;
        pe.index = index;
        pe.distance = 0; //initialized in simulation
        aliveParticleIndex.Append(pe);

        InterlockedAdd(indirectDispatchArgs[0], 1);
    }

}
