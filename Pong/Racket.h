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
    float velocity_magn_x = .0f;
    float velocity_magn_y = .0f;
    float max_velocity = 3.f;
    float angle_velocity = 0.f;
    float curr_angle = 0.f;

    DirectX::XMVECTOR normal = { 1.0f, .0f, 0.f, 0.f };

    Racket();
    Racket(DirectX::XMFLOAT4 position, float height);
    Racket(DirectX::XMFLOAT4 position, float height, float width);
    Racket(DirectX::XMFLOAT4 position, float height, float width, DirectX::XMFLOAT4 color);

    Racket(DirectX::XMFLOAT4* points);

    void Update(float deltaTime);
    void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context,
        ID3D11RenderTargetView* renderTargetView);

    void HitBall(Ball* ball, float deltaTime);
    void Move(float velocity_magn_y);
    
    DirectX::BoundingOrientedBox GetBoundingBox() const;

    void GetCenterLocation(DirectX::XMFLOAT3* loc);
    void MoveUp();
    void MoveDown();
    void MoveLeft();
    void MoveRight();
    void Stop();

    void Normalize();

    DirectX::XMVECTOR ReflectVector(const DirectX::XMVECTOR& normalVec, const DirectX::XMVECTOR& incidentVec);
    bool isFaced(const DirectX::XMVECTOR& v);
    DirectX::XMVECTOR NormalDistribution(float value);
};

#endif

