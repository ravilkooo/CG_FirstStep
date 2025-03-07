#ifndef CAMERA_H
#define CAMERA_H

#include <DirectXMath.h>

class Camera
{
public:
    Camera();
    ~Camera();

    void SetPosition(DirectX::XMFLOAT3 position);
    void SetTarget(DirectX::XMFLOAT3 target);
    void SetUp(DirectX::XMFLOAT3 up);

    void SetFOV(float fov);
    void SetAspectRatio(float aspectRatio);
    void SetNearZ(float nearZ);
    void SetFarZ(float farZ);

    void Update(float deltaTime);

    DirectX::XMMATRIX GetViewMatrix() const;
    DirectX::XMMATRIX GetProjectionMatrix() const;

    void MoveForward(float speed);
    void MoveBackward(float speed);
    void MoveLeft(float speed);
    void MoveRight(float speed);
    void MoveUp(float speed);
    void MoveDown(float speed);

    void RotateYaw(float angle);
    void RotatePitch(float angle);

    void SwitchToOrbitalMode(DirectX::XMFLOAT3 target);
    void SwitchToFPSMode();

private:
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT3 target;
    DirectX::XMFLOAT3 up;

    float fov;
    float aspectRatio;
    float nearZ;
    float farZ;

    bool isOrbitalMode;
    DirectX::XMFLOAT3 orbitalTarget;
    float orbitalDistance;
    float orbitalYaw;
    float orbitalPitch;
};

#endif // CAMERA_H