#ifndef SCENENODE_H
#define SCENENODE_H

class SceneNode
{
public:
    SceneNode();
    virtual ~SceneNode();

    virtual void Update(float deltaTime);
    virtual void Draw();

protected:
    // Позиция, поворот, масштаб и другие свойства
};

#endif // SCENENODE_H