#ifndef BALL_H
#define BALL_H

#include "SceneNode.h"
#include "SimpleMath.h"


class Ball : public SceneNode
{
public:
    DirectX::XMFLOAT4 position = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
    float width = 0.1;

    INT direction_x = -1;
    INT direction_y = -1;
    float velocity = 1;

    Ball();
    Ball(DirectX::XMFLOAT4 position, float width);
    Ball(DirectX::XMFLOAT4* points);

    void Update(float deltaTime);
    void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context,
        ID3D11RenderTargetView* renderTargetView);

    DirectX::BoundingBox GetBoundingBox() const;
};

#endif
