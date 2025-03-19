#include "GravitationBody.h"




Vector3 GravitationBody::CalcForceBetween_noMass(GravitationBody other)
{
    float _dist = Vector3::Distance(other.position, position);
    if (_dist > (radius + other.radius))
        return g_const * (other.position - position) / pow(Vector3::Distance(other.position, position), 3);
    else
        return Vector3::Zero;
}

GravitationBody::GravitationBody(float radius, float spinSpeed,
    XMFLOAT3 position,
    XMFLOAT4 col, PLANET_TYPE planet_type, float density)
    : radius(radius), spinSpeed(spinSpeed), position(position), rotationAngle(0.0f), mass(radius*radius*radius * density)
{

    switch (planet_type)
    {
    case GravitationBody::PLANET_TYPE::CUBE:
    {
        CreateSimpleCubeMesh(radius, radius, radius, col, &vertices, &verticesNum, &indices, &indicesNum);

        auto col_2 = XMFLOAT4(0.5f + 0.5f * col.x, 0.5f + 0.5f * col.y, 0.5f + 0.5f * col.z, 1.0f);
        vertices[0].color = col_2;
        vertices[2].color = col_2;
        vertices[5].color = col_2;
        vertices[7].color = col_2;
    }
    break;

    case GravitationBody::PLANET_TYPE::SPHERE:
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
    case GravitationBody::PLANET_TYPE::GEOSPHERE:
    {
        CreateSimpleGeosphereMesh(radius, col, &vertices, &verticesNum, &indices, &indicesNum);

        auto col_2 = XMFLOAT4(0.5f + 0.5f * col.x, 0.5f + 0.5f * col.y, 0.5f + 0.5f * col.z, 1.0f);
        for (size_t i = 0; i < verticesNum; i += 2)
        {
            vertices[i].color = col_2;
        }
    }
    break;
    case GravitationBody::PLANET_TYPE::RING:
    {
        UINT sliceCount = 10;

        CreateRingMesh(radius, 0.0001, radius * 0.1f, sliceCount, col, &vertices, &verticesNum, &indices, &indicesNum);
        auto col_2 = XMFLOAT4(0.5f + 0.5f * col.x, 0.5f + 0.5f * col.y, 0.5f + 0.5f * col.z, 1.0f);
        for (size_t i = 0; i < sliceCount; i++)
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

GravitationBody::~GravitationBody()
{
}



void GravitationBody::Update(float deltaTime)
{
    velocity += acceleration * deltaTime;
    float velLen = velocity.Length();
    if (velLen > maxVelocity) {
        velocity *= maxVelocity / velLen;
    }
    position += velocity * deltaTime;


    // Обновление матрицы преобразования
    //XMMATRIX rotationXMat = XMMatrixRotationX(rotationAngle);
    XMMATRIX spinRotationMat = Matrix::CreateFromAxisAngle(spinAxis, rotationAngle);
    XMMATRIX translationMat = XMMatrixTranslation(position.x, position.y, position.z);
    worldMat = spinRotationMat * translationMat;
    //cb.wvpMat = translationMat;

    /*Matrix viewMat = camera->GetViewMatrix();
    Matrix projMat = camera->GetProjectionMatrix();


    cb.wvpMat = worldMat * (XMMATRIX) (viewMat * projMat);*/

}

Vector3 GravitationBody::GetCenterLocation()
{
    Vector3 center = Vector3(position);
    return center;
}

