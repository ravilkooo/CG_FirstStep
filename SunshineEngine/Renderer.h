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

#include "Camera.h"

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

    ShaderManager shaderManager;
    ResourceManager resourceManager;

    Camera camera;
private:
    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
    ID3D11Texture2D* pDepthStencil;
    ID3D11DepthStencilView* pDSV;

    TextureManager textureManager;
    Microsoft::WRL::ComPtr<ID3D11Device> device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
    Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
    ID3D11RenderTargetView* renderTargetView;

    D3D_FEATURE_LEVEL featureLevels[1] = { D3D_FEATURE_LEVEL_11_1 };

    InputAssembler inputAssembler;
    PipelineState pipelineState;
    
    DisplayWindow* displayWindow;

    int screenWidth = 800;
    int screenHeight = 800;

    std::chrono::time_point<std::chrono::steady_clock> PrevTime;
    float totalTime;

    //ID3D11InputLayout* layout;
};

#endif // RENDERER_H