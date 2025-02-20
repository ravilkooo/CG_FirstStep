#ifndef SCENENODE_H
#define SCENENODE_H

#include <d3d11.h>
#include <directxmath.h>

class SceneNode
{
public:
    SceneNode();
    virtual ~SceneNode();

    virtual void Update(float deltaTime);
    virtual void Draw();

    LPCWSTR shaderFilePath;
    int* indices;
    DirectX::XMFLOAT4* points;
    UINT pointsNum;
    UINT indicesNum;
    

protected:
    // Позиция, поворот, масштаб и другие свойства
};

#endif // SCENENODE_H