#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include "SceneNode.h"

template<class T>
class Scene
{
public:
    Scene();
    ~Scene();
    
    void AddNode(SceneNode<T>* node);

    void RemoveNode(SceneNode<T>* node);

    void Update(float deltaTime);
    void Draw();

    static std::vector<SceneNode<T>*> nodes;
private:
};

#endif // SCENE_H