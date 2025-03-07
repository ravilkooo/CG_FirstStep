#include "CosmicBody.h"

CosmicBody::CosmicBody(float radius, float rotationSpeed,
    DirectX::XMFLOAT4 position, DirectX::XMFLOAT4 col,
    CosmicBody* attractedTo, float orbitRadius, float orbitSpeed)
    : radius(radius), rotationSpeed(rotationSpeed), position(position),
    orbitAngle(0.0f), rotationAngle(0.0f),
    attractedTo(attractedTo), orbitRadius(orbitRadius), orbitSpeed(orbitSpeed)
{
    float w2 = 0.5f * radius;
    float h2 = 0.5f * radius;
    float d2 = 0.5f * radius;
    
    pointsNum = 8*2;
    indicesNum = 36;
    CreateSimpleCubeMesh(radius, radius, radius, &points, &indices, col);

    //position.z = 3 * radius;
    /*
    int _ind[6] = { 0, 1, 2, 1, 0, 3 };
    indices = (int*)malloc(6 * sizeof(int));
    indicesNum = 6;
    for (int i = 0; i < 6; i++)
    {
        indices[i] = _ind[i];
    }

    DirectX::XMFLOAT4 _points[8] = {
        DirectX::XMFLOAT4(radius, radius, radius, 1.0f),	DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        DirectX::XMFLOAT4(-radius, -radius, radius, 1.0f),	DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        DirectX::XMFLOAT4(radius, -radius, radius, 1.0f),	DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        DirectX::XMFLOAT4(-radius, radius, radius, 1.0f),	DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
    };
    points = (DirectX::XMFLOAT4*)malloc(8 * sizeof(DirectX::XMFLOAT4));
    pointsNum = 8;

    for (int i = 0; i < 8; i++)
    {
        points[i] = _points[i];
    }
    */
    shaderFilePath = L"./Shaders/CubeShader.hlsl";
}

CosmicBody::~CosmicBody()
{
}

void CosmicBody::Update(float deltaTime)
{
    // Обновление угла вращения вокруг своей оси
    rotationAngle += rotationSpeed * deltaTime;
    //rotationAngle = 0.0f;
    if (rotationAngle > DirectX::XM_2PI) {
        rotationAngle -= DirectX::XM_2PI;
    }

    // Обновление матрицы преобразования
    DirectX::XMMATRIX rotationXMat = DirectX::XMMatrixRotationX(rotationAngle);
    DirectX::XMMATRIX rotationYMat = DirectX::XMMatrixRotationY(DirectX::XM_PIDIV4);
    DirectX::XMMATRIX translationMat = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
    cb.wvpMat = rotationXMat * rotationYMat * translationMat;

    // Обновление угла орбиты
    if (attractedTo)
    {
        orbitAngle += orbitSpeed * deltaTime;
        DirectX::XMFLOAT3 orbitCenter;
        attractedTo->GetCenterLocation(&orbitCenter);
        DirectX::XMMATRIX translationToOrbit = DirectX::XMMatrixTranslation(orbitRadius, 0.f, 0.f);
        DirectX::XMMATRIX translationToSunMat = DirectX::XMMatrixTranslation(orbitCenter.x, orbitCenter.y, orbitCenter.z);
        //position.x = orbitCenter.x + radius * cos(orbitAngle);
        //position.z = orbitCenter.z + radius * sin(orbitAngle);
        auto orbitRotate = DirectX::XMMatrixRotationY(orbitAngle);
        cb.wvpMat = cb.wvpMat * translationToOrbit * orbitRotate * translationToSunMat;

        /*if (attractedTo->attractedTo) {
            cb.wvpMat = cb.wvpMat * attractedTo->attractedTo->cb.wvpMat;
        }*/
    }

    /*for (size_t i = 0; i < 8; i++)
    {
        std::cout << points[2*i].x << points[2 * i].y << points[2 * i].z << "; ";
    }
    std::cout << "\n";*/
}

void CosmicBody::SetOrbitSpeed(float speed)
{
    orbitSpeed = speed;
}

void CosmicBody::SetRotationSpeed(float speed)
{
    rotationSpeed = speed;
}

void CosmicBody::GetCenterLocation(DirectX::XMFLOAT3* loc)
{
    DirectX::XMVECTOR center = DirectX::XMVectorSet(position.x,
        position.y,
        position.z, 1.0f);
    DirectX::XMStoreFloat3(loc, DirectX::XMVector4Transform(center, cb.wvpMat));
    return;
}

