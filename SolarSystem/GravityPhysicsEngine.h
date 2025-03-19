#pragma once
#include <PhysicsEngine.h>
class GravityPhysicsEngine :
    public PhysicsEngine
{
    Scene* scene;

    GravityPhysicsEngine();
    GravityPhysicsEngine(Scene* scene);

};

