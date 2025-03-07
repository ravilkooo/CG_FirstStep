#ifndef SCENENODE_H
#define SCENENODE_H

#include <d3d11.h>
#include <directxmath.h>
#include "ShaderManager.h"
#include "ResourceManager.h"

class SceneNode
{
public:
    SceneNode();
    virtual ~SceneNode();

    virtual void Update(float deltaTime) = 0;
    void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context,
        ID3D11RenderTargetView* renderTargetView,
        ID3D11DepthStencilView* pDSV);

    void InitBuffers(ResourceManager resourceManager);

    void LoadAndCompileShader(ShaderManager shaderManager);

    LPCWSTR shaderFilePath;
    int* indices;
    DirectX::XMFLOAT4* points;
    UINT pointsNum;
    UINT indicesNum;
  
    ID3D11VertexShader* vertexShader = nullptr;
    ID3D11PixelShader* pixelShader = nullptr;

    ID3DBlob* vsBlob = nullptr;

    ID3D11Buffer* pIndexBuffer;
    ID3D11Buffer* pVertexBuffer;
    ID3D11Buffer* pConstantBuffer;

    DirectX::XMMATRIX worldMat = DirectX::XMMatrixIdentity();   // Мировая матрица
    DirectX::XMMATRIX viewMat = DirectX::XMMatrixIdentity();    // Матрица вида (камеры)
    DirectX::XMMATRIX projMat = DirectX::XMMatrixIdentity();    // Проекционная матрица

    struct ConstantBuffer
    {
        DirectX::XMMATRIX wvpMat = DirectX::XMMatrixIdentity();     // Комбинированная матрица (world * view * proj)
    };

    ConstantBuffer cb;

    void SetWorldMatrix(const DirectX::XMMATRIX& worldMatrix);
    void SetViewMatrix(const DirectX::XMMATRIX& viewMatrix);
    void SetProjectionMatrix(const DirectX::XMMATRIX& projectionMatrix);


protected:
    // Позиция, поворот, масштаб и другие свойства
    DirectX::XMMATRIX localMatrix = DirectX::XMMatrixIdentity();
};

#endif // SCENENODE_H