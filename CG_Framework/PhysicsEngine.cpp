#include "PhysicsEngine.h"

PhysicsEngine::PhysicsEngine()
{
}

PhysicsEngine::~PhysicsEngine()
{
}

void PhysicsEngine::AddNode(SceneNode* node)
{
    nodes.push_back(node);
}

void PhysicsEngine::RemoveNode(SceneNode* node)
{
    nodes.erase(std::remove(nodes.begin(), nodes.end(), node), nodes.end());
}

void PhysicsEngine::Update(float deltaTime)
{
    for (auto node : nodes)
    {
        // Обновление физики для каждого узла
    }
}