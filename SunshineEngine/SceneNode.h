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
    virtual void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context,
        ID3D11RenderTargetView* renderTargetView) = 0;

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

    struct ConstantBuffer
    {
        DirectX::XMMATRIX wvpMat = DirectX::XMMatrixIdentity();
    };

    ConstantBuffer cb;


protected:
    // Позиция, поворот, масштаб и другие свойства
};

#endif // SCENENODE_H