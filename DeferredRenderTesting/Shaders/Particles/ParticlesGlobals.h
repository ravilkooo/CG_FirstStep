
struct Particle
{
    float4 position;
    float4 velocity;
    float4 color;
    float size;
    float age;
    float lifeSpan;
    float padding;
};

struct ParticleIndexElement
{
    float distance;
    float index;
};
