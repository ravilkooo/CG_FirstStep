#ifndef PHYSICSENGINE_H
#define PHYSICSENGINE_H

#include <vector>
#include "SceneNode.h"

class PhysicsEngine
{
public:
    PhysicsEngine();
    ~PhysicsEngine();

    void AddNode(SceneNode* node);
    void RemoveNode(SceneNode* node);
    void Update(float deltaTime);

private:
    std::vector<SceneNode*> nodes;
};

#endif // PHYSICSENGINE_H