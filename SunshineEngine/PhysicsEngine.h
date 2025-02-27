#ifndef PHYSICSENGINE_H
#define PHYSICSENGINE_H

#include <vector>
#include "Scene.h"

class PhysicsEngine
{
public:
    PhysicsEngine();
    PhysicsEngine(Scene* scene);
    ~PhysicsEngine();
    
    virtual void Update(float deltaTime);

    Scene* scene;
};

#endif // PHYSICSENGINE_H