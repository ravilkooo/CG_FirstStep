#include "CosmicBody.h"

CosmicBody::CosmicBody(float radius, float rotationSpeed,
    XMFLOAT3 position, XMFLOAT4 col, PLANET_TYPE planet_type,
    CosmicBody* attractedTo, float orbitRadius, float orbitSpeed)
    : radius(radius), rotationSpeed(rotationSpeed), position(position),
    orbitAngle(0.0f), rotationAngle(0.0f),
    attractedTo(attractedTo), orbitRadius(orbitRadius), orbitSpeed(orbitSpeed)
{
    float w2 = 0.5f * radius;
    float h2 = 0.5f * radius;
    float d2 = 0.5f * radius;
    
    verticesNum = 8;
    indicesNum = 36;

    switch (planet_type)
    {
    case CosmicBody::PLANET_TYPE::CUBE:
        CreateSimpleCubeMesh(radius, radius, radius, col, &vertices, &verticesNum, &indices, &indicesNum);
        break;
    case CosmicBody::PLANET_TYPE::SPHERE:
        CreateSimpleSphereMesh(radius, 20, 10, col, &vertices, &verticesNum, &indices, &indicesNum);
        break;
    case CosmicBody::PLANET_TYPE::GEOSPHERE:
        CreateSimpleGeosphereMesh(radius, col, &vertices, &verticesNum, &indices, &indicesNum);
        break;
    default:
        break;
    }
    //CreateSimpleCubeMesh(radius, radius, radius, col, &vertices, &verticesNum, &indices, &indicesNum);
    //CreateSimpleGeosphereMesh(radius, col, &vertices, &verticesNum, &indices, &indicesNum);
    //CreateSimpleSphereMesh(radius, 6, 3, col, &vertices, &verticesNum, &indices, &indicesNum);

    IALayoutInputElements = (D3D11_INPUT_ELEMENT_DESC*) malloc(2 * sizeof(D3D11_INPUT_ELEMENT_DESC));
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
        D3D11_INPUT_ELEMENT_DESC {
            "COLOR",
            0,
            DXGI_FORMAT_R32G32B32A32_FLOAT,
            0,
            D3D11_APPEND_ALIGNED_ELEMENT,
            D3D11_INPUT_PER_VERTEX_DATA,
            0};

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
    if (rotationAngle > XM_2PI) {
        rotationAngle -= XM_2PI;
    }

    // Обновление матрицы преобразования
    XMMATRIX rotationXMat = XMMatrixRotationX(rotationAngle);
    XMMATRIX rotationYMat = XMMatrixRotationY(rotationAngle);
    XMMATRIX translationMat = XMMatrixTranslation(position.x, position.y, position.z);
    cb.wvpMat = rotationXMat * rotationYMat * translationMat;
    //cb.wvpMat = translationMat;

    // Обновление угла орбиты
    if (attractedTo)
    {
        orbitAngle += orbitSpeed * deltaTime;
        auto _attractredTransform = GetAttractedToTransform();
        for (size_t i = 0; i < 4; i++)
            for (size_t j = 0; j < 4; j++)
                std::cout << _attractredTransform.m[i][j] << (j == 3 ? "\n" : " ");

        cb.wvpMat = cb.wvpMat * (XMMATRIX) _attractredTransform;
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

Matrix CosmicBody::GetAttractedToTransform() {
    Matrix res = Matrix::Identity;
    if (attractedTo)
    {
        //std::cout << "-";
        Matrix toOrbit = Matrix::CreateTranslation(Vector3(orbitRadius, 0.0f, 0.0f));

        Quaternion q_orbitRot = Quaternion::CreateFromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), orbitAngle);
        Matrix m_orbitRot = Matrix::CreateFromQuaternion(q_orbitRot);
        //Matrix m_orbitRot = Matrix::CreateRotationY(orbitAngle);

        Matrix toAttractedCenter = Matrix::CreateTranslation(attractedTo->GetCenterLocation());

        res = toOrbit * m_orbitRot * toAttractedCenter;
    }
    return res;
}

Vector3 CosmicBody::GetCenterLocation()
{
    /*XMVECTOR center = XMVectorSet(position.x,
        position.y,
        position.z, 1.0f);*/
    Vector3 center = Vector3(position);
    if (attractedTo)
    {
        center = Vector3::Transform(center, GetAttractedToTransform());
        //center = XMVectorAdd(center, _to_attracted);
    }
    return center;
}

