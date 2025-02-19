#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include "SceneNode.h"

class Scene
{
public:
    Scene();
    ~Scene();

    void AddNode(SceneNode* node);
    void RemoveNode(SceneNode* node);
    void Update(float deltaTime);
    void Draw();

private:
    std::vector<SceneNode*> nodes;
};

#endif // SCENE_H