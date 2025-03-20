#ifndef SCENENODE_H
#define SCENENODE_H

#include <d3d11.h>
#include <directxmath.h>
#include <SimpleMath.h>
#include "ShaderManager.h"
#include "ResourceManager.h"
#include "Camera.h"
#include "Texture.h"
#include "Sampler.h"
#include "CommonVertex.h"
#include <assimp/scene.h>

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

    CommonVertex* vertices;
    UINT verticesNum;

    int* indices;
    UINT indicesNum;
  
    D3D11_INPUT_ELEMENT_DESC* IALayoutInputElements;
    UINT numInputElements = 2;

    LPCWSTR shaderFilePath;
    ID3D11VertexShader* vertexShader = nullptr;
    ID3D11PixelShader* pixelShader = nullptr;

    ID3DBlob* vsBlob = nullptr;

    ID3D11Buffer* pIndexBuffer;
    ID3D11Buffer* pVertexBuffer;
    ID3D11Buffer* pConstantBuffer;

    DirectX::XMMATRIX worldMat = DirectX::XMMatrixIdentity();   // ������� �������
    DirectX::XMMATRIX viewMat = DirectX::XMMatrixIdentity();    // ������� ���� (������)
    DirectX::XMMATRIX projMat = DirectX::XMMatrixIdentity();    // ������������ �������

    struct ConstantBuffer
    {
        DirectX::XMMATRIX wvpMat = DirectX::XMMatrixIdentity();     // ��������������� ������� (world * view * proj)
    };

    ConstantBuffer cb;

    void SetWorldMatrix(const DirectX::XMMATRIX& worldMatrix);
    void SetViewMatrix(const DirectX::XMMATRIX& viewMatrix);
    void SetProjectionMatrix(const DirectX::XMMATRIX& projectionMatrix);

    void InitTextures(std::vector<Texture>& textures);

    Camera* camera;

    bool hasTexture = false;
    std::vector<Texture> textures;
    Sampler* textureSampler;

    // void AddChild(SceneNode* child) { children.push_back(child); }
    // const std::vector<SceneNode*>& GetChildren() const { return children; }

    const UINT VertexStride() const;
    const UINT* VertexStridePtr() const;

    std::vector<Texture> LoadMaterialTextures(aiMaterial* pMaterial, aiTextureType textureType, const aiScene* pScene);
    TextureStorageType DetermineTextureStorageType(const aiScene* pScene, aiMaterial* pMat, unsigned int index, aiTextureType textureType);

    ID3D11Device* device;

    std::string directory = "";


protected:
    // std::vector<SceneNode*> children;
    // SceneNode* parentNode;
    DirectX::XMMATRIX localMatrix = DirectX::XMMatrixIdentity();

private:
    UINT vertexStride = sizeof(CommonVertex);
};

#endif // SCENENODE_H