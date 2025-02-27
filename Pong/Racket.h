#ifndef RACKET_H
#define RACKET_H

#include <DirectXCollision.h>

#include "SceneNode.h"
#include "Ball.h"

class Racket : public SceneNode
{
public:
    DirectX::XMFLOAT4 position = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
    float width = 0.1; float height = 0.1;

    //INT direction = 0.f;
    float velocity = .0f;
    float max_velocity = 3.f;

    Racket();
    Racket(DirectX::XMFLOAT4 position, float height);
    Racket(DirectX::XMFLOAT4 position, float height, float width);
    Racket(DirectX::XMFLOAT4* points);

    Racket(DirectX::XMFLOAT4 position, float height, float width, DirectX::XMFLOAT4 color);

    void Update(float deltaTime);
    void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context,
        ID3D11RenderTargetView* renderTargetView);

    void HitBall(Ball* ball);
    void Move(float velocity);
    
    DirectX::BoundingBox GetBoundingBox() const;

    void GetCenterLocation(DirectX::XMFLOAT3* loc);
    void MoveUp();
    void MoveDown();
    void Stop();
};

#endif

