#include "Scene.h"

template <class T>
Scene<T>::Scene()
{
}

template <class T>
Scene<T>::~Scene()
{
    for (auto node : nodes)
    {
        delete node;
    }
}

template <class T>
void Scene<T>::AddNode(SceneNode<T>* node)
{
    nodes.push_back(node);
}

template <class T>
void Scene<T>::RemoveNode(SceneNode<T>* node)
{
    nodes.erase(std::remove(nodes.begin(), nodes.end(), node), nodes.end());
}

template <class T>
void Scene<T>::Update(float deltaTime)
{
    for (auto node : nodes)
    {
        node->Update(deltaTime);
    }
}

template <class T>
void Scene<T>::Draw()
{
    for (auto node : nodes)
    {
        //node->Draw();
    }
}