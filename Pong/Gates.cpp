#include "Gates.h"

Gates::Gates()
{
}

Gates::Gates(DirectX::XMFLOAT4 position, float racket_width) : position(position), racket_width(racket_width)
{
    int _ind[12] = { 0, 1, 2, 1, 0, 3 };
    indices = (int*)malloc(6 * sizeof(int));
    indicesNum = 6;
    for (int i = 0; i < 6; i++)
    {
        indices[i] = _ind[i];
    }

    points = (DirectX::XMFLOAT4*)malloc(8 * sizeof(DirectX::XMFLOAT4));
    pointsNum = 8;
    if (racket_width > 0)
    {
        this->position.x += -width * 0.5 - racket_width * 1.5;
    }
    else
    {
        this->racket_width = -this->racket_width;
        this->position.x += width * 0.5 - racket_width * 1.5;
    }
    DirectX::XMFLOAT4 _points[8] = {
        DirectX::XMFLOAT4(this->position.x + (width + this->racket_width) * 0.5f, 1.f, position.z + 0.0f, position.w + 0.0f),  DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f),
        DirectX::XMFLOAT4(this->position.x - (width + this->racket_width) * 0.5f, -1.f, position.z + 0.0f, position.w + 0.0f),  DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f),
        DirectX::XMFLOAT4(this->position.x + (width + this->racket_width) * 0.5f, -1.f, position.z + 0.0f, position.w + 0.0f),  DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f),
        DirectX::XMFLOAT4(this->position.x - (width + this->racket_width) * 0.5f, 1.f, position.z + 0.0f, position.w + 0.0f),  DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f),
    };
    for (int i = 0; i < 8; i++)
    {
        points[i] = _points[i];
    }

    shaderFilePath = L"./Shaders/MyVeryFirstShader.hlsl";
}

//Gates::Gates(float width, float height)
//{
//  int _ind[6] = { 0, 1, 2, 1, 0, 3 };
//  indices = (int*)malloc(6 * sizeof(int));
//  indicesNum = 6;
//  for (int i = 0; i < 6; i++)
//  {
//    indices[i] = _ind[i];
//  }
//
//
//  width = width > 0 ? width : 0.01;
//  height = height > 0 ? height : 0.01;
//  DirectX::XMFLOAT4 _points[8] = {
//    DirectX::XMFLOAT4(position.x + width, position.y + height, position.z + 0.0f, position.w + 0.0f),  DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f),
//    DirectX::XMFLOAT4(position.x, position.y, position.z + 0.0f, position.w + 0.0f),  DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f),
//    DirectX::XMFLOAT4(position.x + width, position.y, position.z + 0.0f, position.w + 0.0f),  DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f),
//    DirectX::XMFLOAT4(position.x, position.y + height, position.z + 0.0f, position.w + 0.0f),  DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f),
//  };
//  points = (DirectX::XMFLOAT4*)malloc(8 * sizeof(DirectX::XMFLOAT4));
//  pointsNum = 8;
//
//  for (int i = 0; i < 8; i++)
//  {
//    points[i] = _points[i];
//  }
//
//  shaderFilePath = L"./Shaders/MyVeryFirstShader.hlsl";
//}

void Gates::Update(float deltaTime)
{
    /*DirectX::XMMATRIX rotZMat = DirectX::XMMatrixRotationZ(0.3f * deltaTime);
    cb.wvpMat = cb.wvpMat * rotZMat;*/

    //std::cout << "H\n";
}

void Gates::Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context,
    ID3D11RenderTargetView* renderTargetView)
{
    context->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);


    // 6. Create Set of Points
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    context->Map(pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    memcpy(mappedResource.pData, &cb, sizeof(cb));
    context->Unmap(pConstantBuffer, 0);

    UINT strides[] = { 32 };
    UINT offsets[] = { 0 };
    context->IASetVertexBuffers(0, 1, &(pVertexBuffer), strides, offsets);

    // 9. Set Vertex and Pixel Shaders
    context->VSSetConstantBuffers(0u, 1u, &pConstantBuffer);

    context->VSSetShader(vertexShader, nullptr, 0);
    context->PSSetShader(pixelShader, nullptr, 0);

    // 11. Set BackBuffer for Output
    context->OMSetRenderTargets(1, &renderTargetView, nullptr);

    // 14. At the End of While (!isExitRequested): Draw the Triangle
    context->DrawIndexed(12, 0, 0);
}

void Gates::HitBall(Ball* ball)
{
    //ball->velocity_x *= -1;
   
    ball->velocity = DirectX::XMVector3Reflect(ball->velocity, normal);
}

DirectX::BoundingBox Gates::GetBoundingBox() const
{

    DirectX::BoundingBox bbox;


    DirectX::XMVECTOR center = DirectX::XMVectorSet(
        position.x,
        0.f,
        position.z, 1.0f);
    DirectX::XMFLOAT3 extents((width + racket_width) * 0.5f,
        1.f,
        0.0f);
    DirectX::XMStoreFloat3(&(bbox.Center), DirectX::XMVector4Transform(center, cb.wvpMat));

    bbox.Extents = extents;
    return bbox;
}
