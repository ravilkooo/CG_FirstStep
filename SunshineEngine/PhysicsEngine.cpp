#include "PhysicsEngine.h"

template<class VertexType>
PhysicsEngine<VertexType>::PhysicsEngine()
{
}

template<class VertexType>
PhysicsEngine<VertexType>::PhysicsEngine(Scene<VertexType>* scene) : scene(scene)
{
}

template<class VertexType>
PhysicsEngine<VertexType>::~PhysicsEngine()
{
}

template<class VertexType>
void PhysicsEngine<VertexType>::Update(float deltaTime)
{
    for (auto node : scene->nodes)
    {
        // Обновление физики для каждого узла
        node->Update(deltaTime);
    }
}