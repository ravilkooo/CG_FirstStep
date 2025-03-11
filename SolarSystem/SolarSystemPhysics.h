#ifndef SOLARSYS_ENGINE_H
#define SOLARSYS_ENGINE_H

#include <PhysicsEngine.h>

class SolarSystemPhysics :
    public PhysicsEngine
{
public:
    class SolarSystemPhysics();
    class SolarSystemPhysics(Scene* scene);
    
    ~SolarSystemPhysics();

    void Update(float deltaTime) override;

    Scene* scene;
};

#endif // !SOLARSYS_ENGINE_H
