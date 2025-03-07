#include "SolarSystemPhysics.h"


SolarSystemPhysics::SolarSystemPhysics()
{
}

SolarSystemPhysics::SolarSystemPhysics(Scene* scene) : scene(scene)
{
}

SolarSystemPhysics::~SolarSystemPhysics()
{
}

void SolarSystemPhysics::Update(float deltaTime)
{
    for (auto node : scene->nodes)
    {
        // Обновление физики для каждого узла
        node->Update(deltaTime);
    }
}
