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
#include "Drawable.h"

#include "BindableCollection.h"


class SceneNode : public Drawable
{
public:
    SceneNode();
    virtual ~SceneNode();

    virtual void Update(float deltaTime) = 0;
    void BindAll(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);

    void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) const noexcept;

    void InitBuffers(ResourceManager resourceManager);

    void LoadAndCompileShader(ShaderManager shaderManager);

    virtual Vector3 GetCenterLocation() = 0;

    CommonVertex* vertices;
    UINT verticesNum;

    int* indices;
    UINT indicesNum;
  
    D3D11_INPUT_ELEMENT_DESC* IALayoutInputElements;
    UINT numInputElements = 2;

    LPCWSTR vertexShaderFilePath;
    LPCWSTR pixelShaderFilePath;
    ID3D11VertexShader* vertexShader = nullptr;
    ID3D11PixelShader* pixelShader = nullptr;

    ID3DBlob* vsBlob = nullptr;

    ID3D11Buffer* pIndexBuffer;
    ID3D11Buffer* pVertexBuffer;
    ID3D11Buffer* pConstantBuffer;

    void UpdateCB(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);

    struct ConstantBuffer
    {
        DirectX::XMMATRIX wvpMat = DirectX::XMMatrixIdentity();     // Комбинированная матрица (world * view * proj)
    };

    ConstantBuffer cb;

    //DirectX::XMMATRIX worldMat = DirectX::XMMatrixIdentity();   // Мировая матрица
    DirectX::XMMATRIX viewMat = DirectX::XMMatrixIdentity();    // Матрица вида (камеры)
    DirectX::XMMATRIX projMat = DirectX::XMMatrixIdentity();    // Проекционная матрица
    
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

    void AddBind(Bind::Bindable* bind);
    std::vector<Bind::Bindable*> bindables;

    DirectX::XMMATRIX GetViewMatrix() const override;
    DirectX::XMMATRIX GetProjectionMatrix() const override;

protected:
    // std::vector<SceneNode*> children;
    // SceneNode* parentNode;
    DirectX::XMMATRIX localMatrix = DirectX::XMMatrixIdentity();

private:
    UINT vertexStride = sizeof(CommonVertex);
};

#endif // SCENENODE_H