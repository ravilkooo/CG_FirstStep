#pragma once
#include <SceneNode.h>
#include <ShapeGenerator.h>
#include <DrawableBase.h>

class StickyBall : public DrawableBase<StickyBall>
{
public:
    StickyBall();
    StickyBall(ID3D11Device* device);

    void Update(float deltaTime) override;
    Vector3 GetCenterLocation() override;

    void PushForward(float deltaTime);
    void AddTurn(float direction, float deltaTime);

    void SlowDown(float deltaTime);

    Vector3 GetMoveDir();
    void UpdateScale();

    void Grow(float deltaTime);

    float scale = 1.0f;
    float radius = 1.0f;
    float radiusGrow = 0.0f;
    float radiusSlow = 0.3f;

    float velocity = 0.0f;
    float maxVelocity = 10.0f;
    float acceleration = 10.0f;

    float spinSpeed = 0.0f;
    float currentSpin = 0.0f;

    float currentSpeed = 0.0f;

    float rotationSpeed = 0.5f;
    float currentRotation = 0.0f;

    //Vector3 forwardDirection = (0.0f, 0.0f, 1.0f);

    XMFLOAT3 position = XMFLOAT3(0.0f, radius, 0.0f);
public:
    struct Ball_PCB {
        XMFLOAT3 cam_pos;
    } ball_pcb;
    Bind::PixelConstantBuffer<Ball_PCB>* pcb;
private:
    Bind::VertexShader* vertexShaderB;
};

