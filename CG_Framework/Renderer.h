#ifndef RENDERER_H
#define RENDERER_H

#include "ShaderManager.h"
#include "TextureManager.h"
#include "Scene.h"
#include "ResourceManager.h"
#include "InputAssembler.h"

#include <d3d11.h>
#include <wrl.h>
#include <directxmath.h>

#include "DisplayWindow.h"
#include "PipelineState.h"

#include <chrono>

class Renderer
{
public:
    Renderer();
    Renderer(DisplayWindow* displayWin);
    ~Renderer();

    bool Initialize(DisplayWindow* displayWin);
    void RenderScene(const Scene& scene);

    ID3D11Device* GetDevice();
    ID3D11DeviceContext* GetDeviceContext();

private:
    ShaderManager shaderManager;
    TextureManager textureManager;
    Microsoft::WRL::ComPtr<ID3D11Device> device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
    Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
    ID3D11RenderTargetView* renderTargetView;

    D3D_FEATURE_LEVEL featureLevels[1] = { D3D_FEATURE_LEVEL_11_1 };

    InputAssembler inputAssembler;
    ResourceManager resourceManager;
    PipelineState pipelineState;
    
    DisplayWindow* displayWindow;

    ID3D11VertexShader* vertexShader = nullptr;
    ID3D11PixelShader* pixelShader = nullptr;

    int screenWidth = 800;
    int screenHeight = 800;

    std::chrono::time_point<std::chrono::steady_clock> PrevTime;
    float totalTime;

    ID3D11Buffer* ib;
    ID3D11Buffer* vb;
    ID3DBlob* vsBlob;

    //ID3D11InputLayout* layout;

    int indices[6] = { 0,1,2, 1,0,3 };
    DirectX::XMFLOAT4 points[8] = {
        DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f),	DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
        DirectX::XMFLOAT4(-0.5f, -0.5f, 0.5f, 1.0f),	DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f),
        DirectX::XMFLOAT4(0.5f, -0.5f, 0.5f, 1.0f),	DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f),
        DirectX::XMFLOAT4(-0.5f, 0.5f, 0.5f, 1.0f),	DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
    };

    ID3D11RasterizerState* rastState;
};

#endif // RENDERER_H