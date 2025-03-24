#include "CosmicBody.h"

CosmicBody::CosmicBody(float radius, float rotationSpeed,
    XMFLOAT3 position,
    XMFLOAT4 col, PLANET_TYPE planet_type,
    CosmicBody* attractedTo, float orbitRadius, float orbitSpeed)
    : radius(radius), rotationSpeed(rotationSpeed), position(position), rotationAngle(0.0f),
    attractedTo(attractedTo), orbitRadius(orbitRadius), orbitSpeed(orbitSpeed), orbitAngle(0.0f), orbitalAxis(0.0f, 1.0f, 0.0f)
{

    switch (planet_type)
    {
    case CosmicBody::PLANET_TYPE::CUBE:
    {
        CreateSimpleCubeMesh(radius, radius, radius, col, &vertices, &verticesNum, &indices, &indicesNum);

        auto col_2 = XMFLOAT4(0.5f + 0.5f * col.x, 0.5f + 0.5f * col.y, 0.5f + 0.5f * col.z, 1.0f);
        vertices[0].color = col_2;
        vertices[2].color = col_2;
        vertices[5].color = col_2;
        vertices[7].color = col_2;
    }
        break;

    case CosmicBody::PLANET_TYPE::SPHERE:
    {

        UINT sliceCount = 10;
        UINT elevationCount = 5;
        CreateSimpleSphereMesh(radius, sliceCount, elevationCount, col, &vertices, &verticesNum, &indices, &indicesNum);
        auto col_2 = XMFLOAT4(0.5f + 0.5f * col.x, 0.5f + 0.5f * col.y, 0.5f + 0.5f * col.z, 1.0f);
        for (size_t i = 0; i < elevationCount * 2 + 1; i += 2)
        {
            for (size_t j = 0; j < sliceCount; j++)
            {
                vertices[1 + i * sliceCount + j].color = col_2;
            }
        }
    }
        break;
    case CosmicBody::PLANET_TYPE::GEOSPHERE:
    {
        CreateSimpleGeosphereMesh(radius, col, &vertices, &verticesNum, &indices, &indicesNum);

        auto col_2 = XMFLOAT4(0.5f + 0.5f * col.x, 0.5f + 0.5f * col.y, 0.5f + 0.5f * col.z, 1.0f);
        for (size_t i = 0; i < verticesNum; i += 2)
        {
            vertices[i].color = col_2;
        }
    }
        break;
    case CosmicBody::PLANET_TYPE::RING:
    {
        UINT sliceCount = 10;

        CreateRingMesh(radius, 0.0001, radius*0.1f, sliceCount, col, &vertices, &verticesNum, &indices, &indicesNum);
        auto col_2 = XMFLOAT4(0.5f + 0.5f * col.x, 0.5f + 0.5f * col.y, 0.5f + 0.5f * col.z, 1.0f);
        for (size_t i = 0; i < sliceCount; i ++)
        {
            vertices[i].color = col_2;
            vertices[i + sliceCount].color = col_2;
        }
    }
    default:
        break;
    }
    //CreateSimpleCubeMesh(radius, radius, radius, col, &vertices, &verticesNum, &indices, &indicesNum);
    //CreateSimpleGeosphereMesh(radius, col, &vertices, &verticesNum, &indices, &indicesNum);
    //CreateSimpleSphereMesh(radius, 6, 3, col, &vertices, &verticesNum, &indices, &indicesNum);

    numInputElements = 2;

    IALayoutInputElements = (D3D11_INPUT_ELEMENT_DESC*)malloc(numInputElements * sizeof(D3D11_INPUT_ELEMENT_DESC));
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

    vertexShaderFilePath = L"./Shaders/CubeShader.hlsl";
    pixelShaderFilePath = L"./Shaders/CubeShader.hlsl";
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
    //XMMATRIX rotationXMat = XMMatrixRotationX(rotationAngle);
    XMMATRIX spinRotationMat = Matrix::CreateFromAxisAngle(spinAxis, rotationAngle);
    XMMATRIX translationMat = XMMatrixTranslation(position.x, position.y, position.z);
    worldMat = spinRotationMat * translationMat;
    //cb.wvpMat = translationMat;

    // Вращение вокруг другого тела, обновление угла орбиты
    if (attractedTo)
    {
        orbitAngle += orbitSpeed * deltaTime;

        auto _attractredTransform = GetAttractedToTransform();

        worldMat = worldMat * (XMMATRIX) _attractredTransform;
    }

    /*Matrix viewMat = camera->GetViewMatrix();
    Matrix projMat = camera->GetProjectionMatrix();


    cb.wvpMat = worldMat * (XMMATRIX) (viewMat * projMat);*/

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
        Matrix toOrbit = Matrix::CreateTranslation(Vector3(orbitRadius, 0.0f, 0.0f));

        Quaternion q_orbitRot = Quaternion::CreateFromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), orbitAngle);
        //Quaternion q_orbitRot = Quaternion::CreateFromAxisAngle(orbitAxis, orbitAngle);
        Matrix m_orbitRot = Matrix::CreateFromQuaternion(q_orbitRot);

        //Matrix m_orbitOffset = Matrix::CreateFromQuaternion(Quaternion::CreateFromYawPitchRoll());

        Matrix toAttractedCenter = Matrix::CreateTranslation(attractedTo->GetCenterLocation());

        res = toOrbit * m_orbitRot * toAttractedCenter;
    }
    return res;
}

Vector3 CosmicBody::GetCenterLocation()
{
    Vector3 center = Vector3(position);
    if (attractedTo)
    {
        center = Vector3::Transform(center, GetAttractedToTransform());
    }
    return center;
}

