#ifndef RENDERER_H
#define RENDERER_H

#include "Scene.h"

#include <d3d11.h>
#include <wrl.h>
#include <directxmath.h>

#include "DisplayWindow.h"
#include "PipelineState.h"

#include "Camera.h"

#include <chrono>

/*
namespace Bind {
    class Bindable;
}*/

class Renderer
{
    friend class Bindable;
public:
    Renderer();
    Renderer(DisplayWindow* displayWin);
    ~Renderer();

    void RenderScene(const Scene& scene);

    void DrawNode(SceneNode* node);

    ID3D11Device* GetDevice();
    ID3D11DeviceContext* GetDeviceContext();

    Camera camera;

    void AddPerFrameBind(Bind::Bindable* bind);
private:
    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
    ID3D11Texture2D* pDepthStencil;
    ID3D11DepthStencilView* pDSV;

    Microsoft::WRL::ComPtr<ID3D11Device> device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
    Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
    ID3D11RenderTargetView* renderTargetView;

    D3D_FEATURE_LEVEL featureLevels[1] = { D3D_FEATURE_LEVEL_11_1 };

    PipelineState pipelineState;
    
    DisplayWindow* displayWindow;

    int screenWidth = 800;
    int screenHeight = 800;

    std::chrono::time_point<std::chrono::steady_clock> PrevTime;
    float totalTime;

    // TextureManager textureManager;
    //ID3D11InputLayout* layout;

    std::vector<Bind::Bindable*> perFrameBindables;
    void BindAll();
};

#endif // RENDERER_H