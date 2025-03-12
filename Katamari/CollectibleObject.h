// CollectibleObject.h
#pragma once
#include "SceneNode.h"
#include "StickyBall.h"

class CollectibleObject : public SceneNode
{
public:
    CollectibleObject(float radius, const DirectX::XMFLOAT3& position);

    void Update(float deltaTime) override;
    Vector3 GetCenterLocation() override;
    bool CheckCollision(StickyBall& ball);
    void AttachToBall(StickyBall* ball);

    float radius;
    bool isAttached = false;

private:
    DirectX::XMFLOAT3 initialPosition;
    StickyBall* attachedBall = nullptr;
    Matrix attachTransform;
};