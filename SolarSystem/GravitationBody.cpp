#include "GravitationBody.h"




Vector3 GravitationBody::CalcGravForceBetween_noOwnMass(GravitationBody other)
{
    return g_const * (other.position - position) / pow(Vector3::Distance(other.position, position), 3);

}

Vector3 GravitationBody::CalcBounceDeltaV(GravitationBody other)
{
    Vector3 r12 = (other.position - position);
    r12.Normalize();
    float v1_proj = r12.Dot(velocity);
    float v2_proj = r12.Dot(other.velocity);
    if (v2_proj - v1_proj > 0) {
        return Vector3::Zero;
    }
    float v1_proj_new = ((mass - other.mass) * v1_proj + 2 * other.mass * v2_proj) / ((mass + other.mass));
    float v2_proj_new = ((other.mass - mass) * v2_proj + 2 * mass * v1_proj) / ((mass + other.mass));
    Vector3 dv1 = -r12 * v1_proj + r12 * v1_proj_new;
    return dv1;
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
            for (size_t j = 0; j < (sliceCount + 1); j++)
            {
                vertices[1 + i * (sliceCount + 1) + j].color = col_2;
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
    
    AddBind(new Bind::Topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
    AddBind(new Bind::VertexBuffer(device, vertices, verticesNum, sizeof(CommonVertex)));
    AddBind(new Bind::IndexBuffer(device, indices, indicesNum));
    vertexShaderB = new Bind::VertexShader(device, L"./Shaders/CubeShader.hlsl");
    AddBind(vertexShaderB);


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
        D3D11_INPUT_ELEMENT_DESC{
            "COLOR",
            0,
            DXGI_FORMAT_R32G32B32A32_FLOAT,
            0,
            D3D11_APPEND_ALIGNED_ELEMENT,
            D3D11_INPUT_PER_VERTEX_DATA,
            0 };

    AddBind(new Bind::InputLayout(device, IALayoutInputElements, numInputElements, vertexShaderB->GetBytecode()));
    AddBind(new Bind::PixelShader(device, L"./Shaders/CubeShader.hlsl"));

    AddBind(new Bind::TransformCBuffer(device, this, 0u));

    D3D11_RASTERIZER_DESC rastDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
    rastDesc.CullMode = D3D11_CULL_BACK;
    rastDesc.FillMode = D3D11_FILL_SOLID;
    AddBind(new Bind::Rasterizer(device, rastDesc));
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

