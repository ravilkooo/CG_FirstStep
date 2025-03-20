#include "StarBox.h"

StarBox::StarBox(float radius, float spinSpeed,
    XMFLOAT3 position,
    XMFLOAT4 col)
    : radius(radius), spinSpeed(spinSpeed), position(position), rotationAngle(0.0f)
{
    {
        CreateSimpleCubeMesh(radius, radius, radius, col, &vertices, &verticesNum, &indices, &indicesNum);

        auto col_2 = XMFLOAT4(0.5f + 0.5f * col.x, 0.5f + 0.5f * col.y, 0.5f + 0.5f * col.z, 1.0f);
        vertices[0].color = col_2;
        vertices[2].color = col_2;
        vertices[5].color = col_2;
        vertices[7].color = col_2;

        for (size_t i = 0; i < indicesNum / 3; i++)
        {
            UINT _t = indices[3 * i];
            indices[3 * i] = indices[3 * i + 1];
            indices[3 * i + 1] = _t;
        }
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
        D3D11_INPUT_ELEMENT_DESC{
            "COLOR",
            0,
            DXGI_FORMAT_R32G32B32A32_FLOAT,
            0,
            D3D11_APPEND_ALIGNED_ELEMENT,
            D3D11_INPUT_PER_VERTEX_DATA,
            0 };

    shaderFilePath = L"./Shaders/StarBoxShader.hlsl";
}

StarBox::~StarBox()
{
}



void StarBox::Update(float deltaTime)
{
    // Обновление матрицы преобразования
    //XMMATRIX rotationXMat = XMMatrixRotationX(rotationAngle);
    XMMATRIX spinRotationMat = Matrix::CreateFromAxisAngle(spinAxis, rotationAngle);
    worldMat = spinRotationMat;
    //cb.wvpMat = translationMat;

    /*Matrix viewMat = camera->GetViewMatrix();
    Matrix projMat = camera->GetProjectionMatrix();


    cb.wvpMat = worldMat * (XMMATRIX) (viewMat * projMat);*/

}

Vector3 StarBox::GetCenterLocation()
{
    Vector3 center = Vector3(position);
    return center;
}
