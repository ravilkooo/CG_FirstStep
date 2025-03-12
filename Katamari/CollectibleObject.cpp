// CollectibleObject.cpp
#include "CollectibleObject.h"
#include "ShapeGenerator.h"

CollectibleObject::CollectibleObject(float radius, const DirectX::XMFLOAT3& position)
    : radius(radius), initialPosition(position)
{
    DirectX::XMFLOAT4 color(0.2f, 0.8f, 0.3f, 1.0f);
    CreateSimpleCubeMesh(radius * 2, radius * 2, radius * 2, color,
        &vertices, &verticesNum, &indices, &indicesNum);
    worldMat = Matrix::CreateTranslation(position);

    IALayoutInputElements = (D3D11_INPUT_ELEMENT_DESC*)malloc(2 * sizeof(D3D11_INPUT_ELEMENT_DESC));
    IALayoutInputElements[0] =
        D3D11_INPUT_ELEMENT_DESC{
            "POSITION",
            0,
            DXGI_FORMAT_R32G32B32_FLOAT,
            0,
            0,
            D3D11_INPUT_PER_VERTEX_DATA,
            0 };

    IALayoutInputElements[1] =
        D3D11_INPUT_ELEMENT_DESC{
            "COLOR",
            0,
            DXGI_FORMAT_R32G32B32A32_FLOAT,
            0,
            D3D11_APPEND_ALIGNED_ELEMENT,
            D3D11_INPUT_PER_VERTEX_DATA,
            0 };

    shaderFilePath = L"./Shaders/CubeShader.hlsl";
}

Vector3 CollectibleObject::GetCenterLocation()
{
    return Vector3(initialPosition);
}

bool CollectibleObject::CheckCollision(StickyBall& ball)
{
    if (isAttached) return false;

    Vector3 ballPos = ball.GetCenterLocation();
    Vector3 objPos = GetCenterLocation();
    float distance = Vector3::Distance(ballPos, objPos);
    return distance < (ball.radius + radius);
}

void CollectibleObject::AttachToBall(StickyBall* ball)
{
    isAttached = true;
    attachedBall = ball;

    attachTransform = worldMat * XMMatrixInverse(nullptr, Matrix::CreateScale(1/ball->radius) * ball->worldMat);
}

void CollectibleObject::Update(float deltaTime)
{
    if (isAttached && attachedBall)
    {
        worldMat = attachTransform * Matrix::CreateScale(1 / attachedBall->radius) * attachedBall->worldMat;
    }
    else {
        worldMat = Matrix::CreateTranslation(initialPosition);
    }

    Matrix viewMat = camera->GetViewMatrix();
    Matrix projMat = camera->GetProjectionMatrix();


    cb.wvpMat = worldMat * (XMMATRIX)(viewMat * projMat);
}