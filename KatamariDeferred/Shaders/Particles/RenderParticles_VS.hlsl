#include "ParticlesGlobals.h"

cbuffer aliveListCountConstantBuffer : register(b3)
{
    uint nbAliveParticles;

    uint3 aliveListPadding;
};

StructuredBuffer<Particle> particleList : register(t0);
StructuredBuffer<ParticleIndexElement> aliveParticlesIndex : register(t1);

struct GeometryShaderInput
{
    float4 Position : SV_POSITION;
    float3 oPosition : TEXCOORD0;
    float  size : TEXCOORD1;
    float4 Color : TEXCOORD2;
    float3 velocity : TEXCOORD3;
    float spin : TEXCOORD4;
    uint   orientation : TEXCOORD5;
    float3 spinAxis : TEXCOORD6;
    //float4 Normal : TEXCOORD2;
    //float4 uvSprite : TEXCOORD6; //x,y for x,y and zw for size
};

GeometryShaderInput main(uint vertexId : SV_VertexID)
{
    GeometryShaderInput output;
    
    uint index = aliveParticlesIndex[nbAliveParticles - vertexId - 1].index;
    
    Particle p = particleList[index];

    output.Position = p.position;
    output.oPosition = p.position.xyz;
    output.velocity = p.velocity.xyz;

    //float alpha = smoothstep(0.0, abs(p.lifeSpan), p.age);
    float alpha = 1 - smoothstep(0.0, abs(p.lifeSpan), p.age);
    output.Color = normalize(lerp(p.colorStart, p.colorEnd, 1.0 - p.age / p.lifeSpan));
    //output.Color.a *= (1 - alpha);
    //output.Normal = p.normal;
   
    output.size = p.sizeStart + alpha * (p.sizeEnd - p.sizeStart);
    //output.size = p.size;
    
    //output.uvSprite = p.uvSprite;
    
    output.orientation = p.orientation;
    if (p.orientation == PARTICLE_ORIENTATION_RANDOM)
    {
        output.spin = p.worldSpin;
        output.spinAxis = p.worldSpinAxis;

    }
    else
    {
        output.spin = p.screenSpin;
    }

    return output;
}