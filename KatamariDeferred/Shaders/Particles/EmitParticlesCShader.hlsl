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
    row_major float4x4 rotMatrix;
    
    float4 emitterPosition;
    float4 colorStart;
    float4 colorEnd;
    
    uint emitterMaxSpawn;
    float particlesLifeSpan;
    float particlesBaseSpeed;
    float particlesMass;
    
    float particleSizeStart;
    float particleSizeEnd;    
    float longitudeMin;
    float longitudeMax;

    float latitudeMax;
    float particleScreenSpinSpeed;
    float particleWorldSpinSpeed;
    uint particleOrientation;
    
    
    //float2 emitterPadding;
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
        uint rng_state_2 = wang_hash(id.x + 3 * rngSeed);
        
        Particle p = (Particle) 0;
        
        p.position = emitterPosition;
        p.position.w = 1.0;

        float colatitude = latitudeMax * rand_xorshift_normalized(rng_state); // 3.1415
        float longitude = longitudeMin
            + (longitudeMax - longitudeMin) * rand_xorshift_normalized(rng_state_2); // 2 * 3.1415
        
        float radius = 1.0f;
        p.velocity.x = radius * sin(colatitude) * cos(longitude);
        p.velocity.z = radius * sin(colatitude) * sin(longitude);
        p.velocity.y = radius * cos(colatitude);
        p.velocity.w = 1.0f;
        
        p.velocity = particlesBaseSpeed * normalize(p.velocity);
        p.velocity = mul(rotMatrix, p.velocity);
        
        p.colorStart = colorStart;
        p.colorEnd = colorEnd;
        
        p.sizeStart = particleSizeStart;
        p.sizeEnd = particleSizeEnd;
        
        p.lifeSpan = particlesLifeSpan;
        p.age = abs(p.lifeSpan); //abs() so if lifetime is infinite ( < 0.0) it's still has a life

        p.screenSpin = 0;
        p.screenSpinSpeed = particleScreenSpinSpeed;
        
        p.worldSpin = 0;
        p.worldSpinSpeed = particleWorldSpinSpeed;
        
        p.worldSpinAxis = float3(0, 0, 0);
        p.mass = particlesMass;
        p.orientation = particleOrientation;
        
        if (particleOrientation == PARTICLE_ORIENTATION_RANDOM)
        {
            uint rng_state_3 = wang_hash(id.x + 7 * rngSeed);
            uint rng_state_4 = wang_hash(id.x + 13 * rngSeed);
         
            float _colatitude = 3.14159265 * rand_xorshift_normalized(rng_state_3); // 3.1415
            float _longitude = 2 * 3.14159265 * rand_xorshift_normalized(rng_state_4); // 2 * 3.1415
        
            p.worldSpinAxis.x = sin(_colatitude) * cos(_longitude);
            p.worldSpinAxis.z = sin(_colatitude) * sin(_longitude);
            p.worldSpinAxis.y = cos(_colatitude);
        }
        
        uint index = deadListBuffer.Consume();
        particleList[index] = p;

        ParticleIndexElement pe;
        pe.index = index;
        pe.distance = 0; //initialized in simulation
        aliveParticleIndex.Append(pe);

        InterlockedAdd(indirectDispatchArgs[0], 1);
    }

}
