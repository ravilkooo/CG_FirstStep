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
    // �������, �������, ������� � ������ ��������
};

#endif // SCENENODE_H