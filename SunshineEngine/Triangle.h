#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "SceneNode.h"

#include <directxmath.h>
#include <d3d11.h>

class Triangle : public SceneNode
{
public:
    Triangle();
    Triangle(DirectX::XMFLOAT4* points);

    void Update(float deltaTime);
    void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context,
        ID3D11RenderTargetView* renderTargetView);
};

#endif // TRIANGLE_H