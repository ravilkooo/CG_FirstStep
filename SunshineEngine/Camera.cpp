#include "Camera.h"

Camera::Camera()
    : position(0.0f, 0.0f, -5.0f), target(0.0f, 0.0f, 0.0f), up(0.0f, 1.0f, 0.0f),
    fov(DirectX::XM_PIDIV4), aspectRatio(16.0f / 9.0f), nearZ(0.1f), farZ(1000.0f),
    isOrbitalMode(false), orbitalTarget(0.0f, 0.0f, 0.0f), orbitalDistance(5.0f),
    orbitalYaw(0.0f), orbitalPitch(0.0f)
{
}

Camera::~Camera()
{
}

void Camera::SetPosition(DirectX::XMFLOAT3 position)
{
    this->position = position;
}

void Camera::SetTarget(DirectX::XMFLOAT3 target)
{
    this->target = target;
}

void Camera::SetUp(DirectX::XMFLOAT3 up)
{
    this->up = up;
}

void Camera::SetFOV(float fov)
{
    this->fov = fov;
}

void Camera::SetAspectRatio(float aspectRatio)
{
    this->aspectRatio = aspectRatio;
}

void Camera::SetNearZ(float nearZ)
{
    this->nearZ = nearZ;
}

void Camera::SetFarZ(float farZ)
{
    this->farZ = farZ;
}

void Camera::Update(float deltaTime)
{
    if (isOrbitalMode)
    {
        // Обновление позиции камеры в орбитальном режиме
        position.x = orbitalTarget.x + orbitalDistance * cos(orbitalYaw) * cos(orbitalPitch);
        position.y = orbitalTarget.y + orbitalDistance * sin(orbitalPitch);
        position.z = orbitalTarget.z + orbitalDistance * sin(orbitalYaw) * cos(orbitalPitch);
    }
}

DirectX::XMMATRIX Camera::GetViewMatrix() const
{
    return DirectX::XMMatrixLookAtLH(
        DirectX::XMLoadFloat3(&position),
        DirectX::XMLoadFloat3(&target),
        DirectX::XMLoadFloat3(&up)
    );
}

DirectX::XMMATRIX Camera::GetProjectionMatrix() const
{
    return DirectX::XMMatrixPerspectiveFovLH(fov, aspectRatio, nearZ, farZ);
}

void Camera::MoveForward(float speed)
{
    DirectX::XMVECTOR forward = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&target), DirectX::XMLoadFloat3(&position));
    forward = DirectX::XMVector3Normalize(forward);
    position.x += speed * DirectX::XMVectorGetX(forward);
    position.y += speed * DirectX::XMVectorGetY(forward);
    position.z += speed * DirectX::XMVectorGetZ(forward);
    target.x += speed * DirectX::XMVectorGetX(forward);
    target.y += speed * DirectX::XMVectorGetY(forward);
    target.z += speed * DirectX::XMVectorGetZ(forward);
}

void Camera::MoveBackward(float speed)
{
    MoveForward(-speed);
}

void Camera::MoveLeft(float speed)
{
    DirectX::XMVECTOR right = DirectX::XMVector3Cross(
        DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&target), DirectX::XMLoadFloat3(&position)),
        DirectX::XMLoadFloat3(&up)
    );
    right = DirectX::XMVector3Normalize(right);
    position.x += speed * DirectX::XMVectorGetX(right);
    position.y += speed * DirectX::XMVectorGetY(right);
    position.z += speed * DirectX::XMVectorGetZ(right);
    target.x += speed * DirectX::XMVectorGetX(right);
    target.y += speed * DirectX::XMVectorGetY(right);
    target.z += speed * DirectX::XMVectorGetZ(right);
}

void Camera::MoveRight(float speed)
{
    MoveLeft(-speed);
}

void Camera::MoveUp(float speed)
{
    position.y += speed;
    target.y += speed;
}

void Camera::MoveDown(float speed)
{
    MoveUp(-speed);
}

void Camera::RotateYaw(float angle)
{
    if (isOrbitalMode)
    {
        orbitalYaw += angle;
    }
    else
    {
        // Поворот камеры в FPS-режиме
    }
}

void Camera::RotatePitch(float angle)
{
    if (isOrbitalMode)
    {
        orbitalPitch += angle;
    }
    else
    {
        // Поворот камеры в FPS-режиме
    }
}

void Camera::SwitchToOrbitalMode(DirectX::XMFLOAT3 target)
{
    isOrbitalMode = true;
    orbitalTarget = target;
    orbitalDistance = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&position), DirectX::XMLoadFloat3(&target))));
}

void Camera::SwitchToFPSMode()
{
    isOrbitalMode = false;
}