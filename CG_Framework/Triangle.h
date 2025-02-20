#ifndef QUAD_H
#define QUAD_H

#include "SceneNode.h"

#include <directxmath.h>
#include <d3d11.h>

class Triangle : public SceneNode
{
public:
    DirectX::XMFLOAT4 points[6];
    
    void Update(float deltaTime);
    void Draw();

    ID3D11Buffer* pVertexBuffer;
    ID3D11Buffer* pIndexBuffer;
};

#endif