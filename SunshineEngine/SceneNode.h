#ifndef SCENENODE_H
#define SCENENODE_H

#include <d3d11.h>
#include <directxmath.h>
#include <SimpleMath.h>
#include "ShaderManager.h"
#include "ResourceManager.h"
#include "Camera.h"

struct Vertex {
    DirectX::XMFLOAT3 pos;
    DirectX::XMFLOAT4 color;
};

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

    virtual Vector3 GetCenterLocation() = 0;

    Vertex* vertices;
    UINT verticesNum;

    int* indices;
    UINT indicesNum;
  
    D3D11_INPUT_ELEMENT_DESC* IALayoutInputElements;

    LPCWSTR shaderFilePath;
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


    Camera* camera;

protected:
    // Позиция, поворот, масштаб и другие свойства
    DirectX::XMMATRIX localMatrix = DirectX::XMMatrixIdentity();
};

#endif // SCENENODE_H