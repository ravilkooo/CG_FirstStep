#include "TestCube.h"

#include <ShapeGenerator.h>


TestCube::TestCube()
{
}


TestCube::TestCube(ID3D11Device* device)
    : TestCube(device, 0.2, 0.2, 0.2, {0,0,0}, {1,0,0,1})
{
}

TestCube::TestCube(ID3D11Device* device, Bind::TextureB* texture)
    : TestCube(device, 0.2, 0.2, 0.2, { 0,0,0 }, { 1,0,0,1 }, texture)
{
}


TestCube::TestCube(ID3D11Device* device, float width, float height, float depth, Vector3 position, Vector4 col)
{
    std::cout << "hi1\n";
    this->device = device;
    this->position = position;

    // DirectX::XMFLOAT4 col = DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

    CreateSimpleCubeMesh(width, height, depth, col, &vertices, &verticesNum, &indices, &indicesNum);

    auto col_2 = XMFLOAT4(0.5f + 0.5f * col.x, 0.5f + 0.5f * col.y, 0.5f + 0.5f * col.z, 1.0f);
    vertices[0].color = col_2;
    vertices[2].color = col_2;
    vertices[5].color = col_2;
    vertices[7].color = col_2;

    if (!IsStaticInitialized()) {
        AddStaticBind(new Bind::Topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
        AddStaticBind(new Bind::IndexBuffer(device, indices, indicesNum));
        vertexShaderB = new Bind::VertexShader(device, L"./Shaders/CubeShaderVS.hlsl");
        AddStaticBind(vertexShaderB);


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

        AddStaticBind(new Bind::InputLayout(device, IALayoutInputElements, numInputElements, vertexShaderB->GetBytecode()));



        AddStaticBind(new Bind::PixelShader(device, L"./Shaders/CubeShaderPS.hlsl"));


        D3D11_RASTERIZER_DESC rastDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
        rastDesc.CullMode = D3D11_CULL_BACK;
        rastDesc.FillMode = D3D11_FILL_SOLID;
        AddStaticBind(new Bind::Rasterizer(device, rastDesc));
    }
    AddBind(new Bind::VertexBuffer(device, vertices, verticesNum, sizeof(CommonVertex)));
    AddBind(new Bind::TransformCBuffer(device, this, 0u));


    shadowPixelCBuffer = new Bind::PixelConstantBuffer<ShadowTransform>(device, 0u);

}

TestCube::TestCube(ID3D11Device* device, float width, float height, float depth, Vector3 position, Vector4 col, Bind::TextureB* texture)
{
    std::cout << "hi1\n";
    this->device = device;
    this->position = position;

    // DirectX::XMFLOAT4 col = DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

    CreateSimpleCubeMesh(width, height, depth, col, &vertices, &verticesNum, &indices, &indicesNum);

    auto col_2 = XMFLOAT4(0.5f + 0.5f * col.x, 0.5f + 0.5f * col.y, 0.5f + 0.5f * col.z, 1.0f);
    vertices[0].color = col_2;
    vertices[2].color = col_2;
    vertices[5].color = col_2;
    vertices[7].color = col_2;

    //if (!IsStaticInitialized())
    {
        std::cout << "hi2\n";
        AddStaticBind(new Bind::Topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
        AddStaticBind(new Bind::IndexBuffer(device, indices, indicesNum));
        AddStaticBind(texture);
        vertexShaderB = new Bind::VertexShader(device, L"./Shaders/CubeShaderVS.hlsl");
        AddStaticBind(vertexShaderB);


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

        AddStaticBind(new Bind::InputLayout(device, IALayoutInputElements, numInputElements, vertexShaderB->GetBytecode()));


        AddStaticBind(new Bind::PixelShader(device, L"./Shaders/CubeShaderPS.hlsl"));


        D3D11_RASTERIZER_DESC rastDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
        rastDesc.CullMode = D3D11_CULL_BACK;
        rastDesc.FillMode = D3D11_FILL_SOLID;
        AddStaticBind(new Bind::Rasterizer(device, rastDesc));
    }
    AddBind(new Bind::VertexBuffer(device, vertices, verticesNum, sizeof(CommonVertex)));
    AddBind(new Bind::TransformCBuffer(device, this, 0u));


    shadowPixelCBuffer = new Bind::PixelConstantBuffer<ShadowTransform>(device, 0u);

}


void TestCube::Update(float deltaTime)
{
    worldMat = initTransform * Matrix::CreateTranslation(position);


}

Vector3 TestCube::GetCenterLocation()
{
    return position;
}

void TestCube::SetInitTransform(Matrix transformMat)
{
    initTransform = transformMat;
}