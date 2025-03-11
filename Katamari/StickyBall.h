#pragma once
#include <SceneNode.h>
#include <ShapeGenerator.h>

class StickyBall :
    public SceneNode
{
public:
    StickyBall();

    void Update(float deltaTime) override;
    Vector3 GetCenterLocation() override;

    void PushForward(float deltaTime);
    void AddTurn(float direction, float deltaTime);

    void SlowDown(float deltaTime);

    Vector3 GetMoveDir();

    float radius = 1.0f;
    float scale = 1.0f;

    float velocity = 0.0f;
    float max_velocity = 10.0f;
    float acceleration = 1.0f;

    float spin_speed = 0.0f;

    float rotation_speed = 0.0f;
    float max_rotation_speed = 1.0f;
    float curr_rotation = 0.0f;
    float max_rotation = XM_PIDIV4;
    float rot_acceleration = 0.5f;

    XMFLOAT3 position = XMFLOAT3(0.0f, radius, 0.0f);
};

