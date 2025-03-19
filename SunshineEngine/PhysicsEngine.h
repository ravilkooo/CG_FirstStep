#ifndef PHYSICSENGINE_H
#define PHYSICSENGINE_H

#include <vector>
#include "Scene.h"


template<class VertexType>
class PhysicsEngine
{
public:
    PhysicsEngine();

    PhysicsEngine(Scene<VertexType>* scene);
    ~PhysicsEngine();
    
    virtual void Update(float deltaTime);

    Scene<VertexType>* scene;
};

#endif // PHYSICSENGINE_H