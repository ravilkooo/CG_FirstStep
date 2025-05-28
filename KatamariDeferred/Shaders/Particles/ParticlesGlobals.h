
struct Particle
{
    float4 position;
    float4 velocity;
    float4 colorStart;
    float4 colorEnd;

    float sizeStart;
    float sizeEnd;
    float age;
    float lifeSpan;

    float screenSpin;
    float screenSpinSpeed;
    float worldSpin;
    float worldSpinSpeed;

    float3 worldSpinAxis;
    float mass;

    uint orientation;
    float3 padding;
};

struct ParticleIndexElement
{
    float distance;
    float index;
};


static uint PARTICLE_ORIENTATION_BILLBOARD = 0;
static uint PARTICLE_ORIENTATION_RANDOM = 1;