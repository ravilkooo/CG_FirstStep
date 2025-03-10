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
    // ���������� ���� �������� ������ ����� ���
    rotationAngle += rotationSpeed * deltaTime;
    //rotationAngle = 0.0f;
    if (rotationAngle > XM_2PI) {
        rotationAngle -= XM_2PI;
    }

    // ���������� ������� ��������������
    //XMMATRIX rotationXMat = XMMatrixRotationX(rotationAngle);
    XMMATRIX rotationYMat = XMMatrixRotationY(rotationAngle);
    XMMATRIX translationMat = XMMatrixTranslation(position.x, position.y, position.z);
    cb.worldMat = rotationYMat * translationMat;
    //cb.wvpMat = translationMat;

    // �������� ������ ������� ����, ���������� ���� ������
    if (attractedTo)
    {
        orbitAngle += orbitSpeed * deltaTime;

        auto _attractredTransform = GetAttractedToTransform();

        cb.worldMat = cb.worldMat * (XMMATRIX) _attractredTransform;
    }

    Matrix viewMat = camera->GetViewMatrix();
    Matrix projMat = camera->GetProjectionMatrix();


    cb.wvpMat = cb.worldMat * (XMMATRIX) (viewMat * projMat);
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
        Matrix m_orbitRot = Matrix::CreateFromQuaternion(q_orbitRot);

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

